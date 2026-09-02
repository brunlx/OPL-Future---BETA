#include "include/opl.h"
#include "include/texcache.h"
#include "include/textures.h"
#include "include/ioman.h"
#include "include/gui.h"
#include "include/util.h"
#include "include/renderman.h"

#include <stdint.h>
#include <delaythread.h>

#define TEXCACHE_POOL_SIZE 32
#define TEXCACHE_MAX_ENTRIES 128

typedef struct {
    char *value;
    image_cache_t *cache;
    cache_entry_t *entry;
    item_list_t *list;
    int cacheUID;
} load_image_request_t;

static void cacheLoadImage(void *data);

// Pool de requisições pré-alocado (evita malloc/free no hot path)
typedef struct {
    load_image_request_t requests[TEXCACHE_POOL_SIZE];
    uint8_t used[TEXCACHE_POOL_SIZE];
    int free_list[TEXCACHE_POOL_SIZE];
    int free_count;
} request_pool_t;

static request_pool_t g_req_pool = {0};

// LRU usando lista duplamente ligada (O(1) insert/remove)
typedef struct lru_node {
    cache_entry_t *entry;
    struct lru_node *prev, *next;
} lru_node_t;

typedef struct {
    image_cache_t *cache;
    lru_node_t nodes[TEXCACHE_MAX_ENTRIES];
    lru_node_t head, tail;
    int node_count;
    uint32_t frame_counter;
    u16 nodes_last_drawn[TEXCACHE_MAX_ENTRIES]; // Frame when each node was last drawn
} lru_cache_t;

static lru_cache_t g_lru_caches[8];
static int g_lru_cache_count = 0;

static void request_pool_init(void)
{
    for (int i = 0; i < TEXCACHE_POOL_SIZE; i++) {
        g_req_pool.free_list[i] = i;
        g_req_pool.used[i] = 0;
    }
    g_req_pool.free_count = TEXCACHE_POOL_SIZE;
}

static load_image_request_t *request_pool_alloc(void)
{
    if (g_req_pool.free_count == 0) return NULL;
    int idx = g_req_pool.free_list[--g_req_pool.free_count];
    g_req_pool.used[idx] = 1;
    return &g_req_pool.requests[idx];
}

static void request_pool_free(load_image_request_t *req)
{
    if (!req)
        return;

    free(req->value);
    req->value = NULL;

    int idx = req - g_req_pool.requests;
    if (idx >= 0 && idx < TEXCACHE_POOL_SIZE && g_req_pool.used[idx]) {
        g_req_pool.used[idx] = 0;
        g_req_pool.free_list[g_req_pool.free_count++] = idx;
    }
}

static void lru_init(lru_cache_t *lru, image_cache_t *cache, int max_entries)
{
    (void)max_entries;
    lru->cache = cache;
    lru->head.next = &lru->tail;
    lru->tail.prev = &lru->head;
    lru->head.prev = lru->tail.next = NULL;
    lru->node_count = 0;
    lru->frame_counter = 0;
    memset(lru->nodes_last_drawn, 0, sizeof(lru->nodes_last_drawn));
}

static void lru_move_to_front(lru_cache_t *lru, lru_node_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = lru->head.next;
    node->prev = &lru->head;
    lru->head.next->prev = node;
    lru->head.next = node;
    if (node->entry)
        node->entry->lastUsed = lru->frame_counter;
}

static void lru_add(lru_cache_t *lru, cache_entry_t *entry)
{
    if (lru->node_count >= TEXCACHE_MAX_ENTRIES) return;
    lru_node_t *node = &lru->nodes[lru->node_count++];
    node->entry = entry;
    if (entry)
        entry->lastUsed = lru->frame_counter;
    lru_move_to_front(lru, node);
}

static cache_entry_t *lru_evict(lru_cache_t *lru)
{
    if (lru->tail.prev == &lru->head) return NULL;

    // Find the node with the oldest lastUsed timestamp for smarter eviction
    // This prioritizes evicting textures that haven't been accessed recently,
    // rather than just the last-added entry (pure LRU)
    lru_node_t *oldest = lru->tail.prev;
    lru_node_t *node = lru->tail.prev->prev;

    while (node != &lru->head) {
        if (node->entry && oldest->entry &&
            node->entry->lastUsed < oldest->entry->lastUsed) {
            oldest = node;
        }
        node = node->prev;
    }

    // Remove oldest from the list
    oldest->prev->next = &lru->tail;
    lru->tail.prev = oldest->prev;
    oldest->prev = NULL;
    oldest->next = NULL;

    return oldest->entry;
}

void cacheInit()
{
    request_pool_init();
    ioRegisterHandler(IO_CACHE_LOAD_ART, &cacheLoadImage);
}

void cacheEnd()
{
}

void cacheAdvanceFrame(void)
{
    for (int i = 0; i < g_lru_cache_count; i++)
        g_lru_caches[i].frame_counter++;
}

static void cacheClearItem(cache_entry_t *item, int freeTxt)
{
    if (freeTxt && item->texture.Mem) {
        rmUnloadTexture(&item->texture);
        free(item->texture.Mem);
        item->texture.Mem = NULL;
        if (item->texture.Clut) {
            free(item->texture.Clut);
            item->texture.Clut = NULL;
        }
    }
    memset(item, 0, sizeof(cache_entry_t));
    item->texture.Mem = NULL;
    item->texture.Vram = 0;
    item->texture.Clut = NULL;
    item->texture.VramClut = 0;
    item->texture.ClutStorageMode = GS_CLUT_STORAGE_CSM1;
    item->qr = NULL;
    item->lastUsed = -1;
    item->UID = 0;
}

image_cache_t *cacheInitCache(int userId, const char *prefix, int isPrefixRelative, const char *suffix, int count)
{
    if (count > TEXCACHE_MAX_ENTRIES) count = TEXCACHE_MAX_ENTRIES;
    if (count < 1) count = 1;

    image_cache_t *cache = (image_cache_t *)malloc(sizeof(image_cache_t));
    if (!cache) return NULL;

    cache->userId = userId;
    cache->count = count;
    cache->prefix = NULL;

    if (prefix) {
        int length = strlen(prefix) + 1;
        cache->prefix = (char *)malloc(length);
        if (cache->prefix) memcpy(cache->prefix, prefix, length);
    }
    cache->isPrefixRelative = isPrefixRelative;

    int length = strlen(suffix) + 1;
    cache->suffix = (char *)malloc(length);
    if (cache->suffix) memcpy(cache->suffix, suffix, length);

    cache->nextUID = 1;
    cache->content = (cache_entry_t *)memalign(64, count * sizeof(cache_entry_t));

    if (!cache->content) {
        free(cache->prefix);
        free(cache->suffix);
        free(cache);
        return NULL;
    }

    for (int i = 0; i < count; ++i)
        cacheClearItem(&cache->content[i], 0);

    if (g_lru_cache_count < 8) {
        lru_init(&g_lru_caches[g_lru_cache_count++], cache, count);
    }

    return cache;
}

void cacheDestroyCache(image_cache_t *cache)
{
    if (!cache) return;

    for (int i = 0; i < cache->count; ++i) {
        cacheClearItem(&cache->content[i], 1);
    }
    free(cache->prefix);
    free(cache->suffix);
    free(cache->content);
    free(cache);
}

static void cacheLoadImage(void *data)
{
    load_image_request_t *req = data;

    if (!req || !req->entry || !req->cache || !req->list) {
        if (req) request_pool_free(req);
        return;
    }

    item_list_t *handler = req->list;

    if (req->cacheUID != req->entry->UID) {
        request_pool_free(req);
        return;
    }

    GSTEXTURE *texture = &req->entry->texture;
    texFree(texture);

    int result = handler->itemGetImage(handler, req->cache->prefix,
                                        req->cache->isPrefixRelative,
                                        req->value, req->cache->suffix,
                                        texture, GS_PSM_CT24);

    if (result < 0) {
        req->entry->lastUsed = 0;
    } else {
        req->entry->lastUsed = guiFrameId;
        for (int i = 0; i < g_lru_cache_count; i++) {
            if (g_lru_caches[i].cache == req->cache) {
                lru_add(&g_lru_caches[i], req->entry);
                break;
            }
        }
    }

    req->entry->qr = NULL;
    request_pool_free(req);
}

GSTEXTURE *cacheGetTexture(image_cache_t *cache, item_list_t *list, int *cacheId, int *UID, char *value)
{
    if (!cache || !list || !value) return NULL;

    if (*cacheId != -1 && *cacheId != -2 && *cacheId < cache->count) {
        cache_entry_t *entry = &cache->content[*cacheId];
        if (entry->UID == *UID) {
            if (!entry->qr && entry->lastUsed > 0) {
                entry->lastUsed = guiFrameId;
                for (int i = 0; i < g_lru_cache_count; i++) {
                    if (g_lru_caches[i].cache == cache) {
                        for (int n = 0; n < g_lru_caches[i].node_count; n++) {
                            if (g_lru_caches[i].nodes[n].entry == entry) {
                                lru_move_to_front(&g_lru_caches[i], &g_lru_caches[i].nodes[n]);
                                break;
                            }
                        }
                        break;
                    }
                }
                return &entry->texture;
            }
            // If lastUsed is 0, the entry was cleared/failed previously; evict it
    // but only if it's not currently being loaded (qr != NULL)
    if (entry->lastUsed == 0 && !entry->qr) {
        *cacheId = -2;
        return NULL;
    }
    // If lastUsed > 0 but entry wasn't found in LRU tracking, still keep it
    // and let the normal eviction handle cache full situations
        }
        *cacheId = -1;
    }

    if (guiInactiveFrames < list->delay)
        return NULL;

    cache_entry_t *oldestEntry = NULL;
    int oldestIdx = -1;

    for (int i = 0; i < g_lru_cache_count; i++) {
        if (g_lru_caches[i].cache == cache) {
            cache_entry_t *victim = lru_evict(&g_lru_caches[i]);
            if (victim) {
                oldestEntry = victim;
                for (int j = 0; j < cache->count; j++) {
                    if (&cache->content[j] == victim) {
                        oldestIdx = j;
                        break;
                    }
                }
            }
            break;
        }
    }

    if (oldestEntry && oldestIdx >= 0) {
        load_image_request_t *req = request_pool_alloc();
        if (!req) return NULL;

        size_t val_len = strlen(value) + 1;
        req->value = (char *)malloc(val_len);
        if (!req->value) {
            request_pool_free(req);
            return NULL;
        }
        memcpy(req->value, value, val_len);

        req->cache = cache;
        req->entry = oldestEntry;
        req->list = list;
        req->cacheUID = cache->nextUID;

        cacheClearItem(oldestEntry, 1);
        oldestEntry->qr = req;
        oldestEntry->UID = cache->nextUID++;

        *cacheId = oldestIdx;
        *UID = oldestEntry->UID;

        if (ioPutRequest(IO_CACHE_LOAD_ART, req) < 0) {
            cacheClearItem(oldestEntry, 0);
            request_pool_free(req);
            *cacheId = -1;
        }
    }

    return NULL;
}