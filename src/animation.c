#include "include/opl.h"
#include "include/renderman.h"
#include "include/gui.h"

typedef enum {
    ANIM_LINEAR,
    ANIM_EASE_IN,
    ANIM_EASE_OUT,
    ANIM_EASE_IN_OUT,
    ANIM_BOUNCE,
    ANIM_ELASTIC
} anim_easing_t;

typedef struct {
    float start_val, end_val;
    float *target_ptr;
    u32 start_frame, duration;
    anim_easing_t easing;
    uint8_t active;
} animation_t;

#define MAX_ANIMATIONS 32
static animation_t g_animations[MAX_ANIMATIONS];
static int g_anim_count = 0;

static float anim_ease_in(float t) { return t * t; }
static float anim_ease_out(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
static float anim_ease_in_out(float t) { return t < 0.5f ? 2*t*t : 1 - powf(-2*t + 2, 2)/2; }
static float anim_bounce(float t) {
    const float n1 = 7.5625f, d1 = 2.75f;
    if (t < 1 / d1)
        return n1 * t * t;
    if (t < 2 / d1) {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    }
    if (t < 2.5f / d1) {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    }
    t -= 2.625f / d1;
    return n1 * t * t + 0.984375f;
}
static float anim_elastic(float t) {
    if (t == 0 || t == 1) return t;
    const float c4 = (2 * 3.14159f) / 3;
    return powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1;
}

static float anim_ease(float t, anim_easing_t easing) {
    switch (easing) {
        case ANIM_EASE_IN: return anim_ease_in(t);
        case ANIM_EASE_OUT: return anim_ease_out(t);
        case ANIM_EASE_IN_OUT: return anim_ease_in_out(t);
        case ANIM_BOUNCE: return anim_bounce(t);
        case ANIM_ELASTIC: return anim_elastic(t);
        default: return t;
    }
}

int anim_start(float *target, float start, float end, u32 duration, anim_easing_t easing) {
    if (g_anim_count >= MAX_ANIMATIONS) return -1;

    animation_t *a = &g_animations[g_anim_count++];
    a->target_ptr = target;
    a->start_val = start;
    a->end_val = end;
    a->start_frame = guiFrameId;
    a->duration = duration;
    a->easing = easing;
    a->active = 1;
    *target = start;
    return g_anim_count - 1;
}

void anim_update_all(void) {
    for (int i = 0; i < g_anim_count; i++) {
        animation_t *a = &g_animations[i];
        if (!a->active) continue;

        u32 elapsed = guiFrameId - a->start_frame;
        if (elapsed >= a->duration) {
            *a->target_ptr = a->end_val;
            a->active = 0;
        } else {
            float t = (float)elapsed / a->duration;
            float eased = anim_ease(t, a->easing);
            *a->target_ptr = a->start_val + (a->end_val - a->start_val) * eased;
        }
    }

    // Compact array removing inactive
    int write = 0;
    for (int read = 0; read < g_anim_count; read++) {
        if (g_animations[read].active) {
            if (write != read) g_animations[write] = g_animations[read];
            write++;
        }
    }
    g_anim_count = write;
}

void anim_stop_all(void) {
    g_anim_count = 0;
}