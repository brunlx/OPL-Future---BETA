#include "gsKit.h"

/* Minimal stub implementations */
void gsKit_init_global(void) {}
void gsKit_hires_init_global(void) {}
void gsKit_init_screen(void) {}
void gsKit_hires_init_screen(void) {}
void gsKit_mode_switch(GsGlobal *global, int mode) { (void)global; (void)mode; }
void gsKit_clear(GsGlobal *global, u32 col) { (void)global; (void)col; }
void gsKit_hires_sync(GsGlobal *global) { (void)global; }
void gsKit_hires_flip(GsGlobal *global) { (void)global; }
void gsKit_finish(GsGlobal *global) { (void)global; }
void gsKit_set_test(GsGlobal *global, int test) { (void)global; (void)test; }
void gsKit_set_primalpha(GsGlobal *global, u32 col, int) { (void)global; (void)col; }
void gsKit_setactive(GsGlobal *global) { (void)global; }
void gsKit_queue_exec(GsGlobal *global) { (void)global; }
void gsKit_TexManager_free(GsGlobal *global, void *txt) { (void)global; (void)txt; }
void gsKit_TexManager_bind(GsGlobal *global, void *txt) { (void)global; (void)txt; }
void gsKit_TexManager_invalidate(GsGlobal *global, void *txt) { (void)global; (void)txt; }
void gsKit_TexManager_nextFrame(GsGlobal *global) { (void)global; }
short int gsKit_check_rom(void) { return 1; }
void gsKit_set_display_offset(GsGlobal *global, int x, int y) { (void)global; (void)x; (void)y; }
void gsKit_prim_sprite(GsGlobal *global, void *q) { (void)global; (void)q; }
void gsKit_prim_sprite_texture(GsGlobal *global, void *q, void *txt) { (void)global; (void)q; (void)txt; }
void gsKit_prim_line(GsGlobal *global, int x1, int y1, int x2, int y2, int order, u32 color) { (void)global; (void)x1; (void)y1; (void)x2; (void)y2; (void)order; (void)color; }
void gsKit_prim_quad_texture(GsGlobal *global, void *inlay, int tx1, int ty1, int tx2, int ty2, void *txt) { (void)global; (void)inlay; (void)tx1; (void)ty1; (void)tx2; (void)ty2; (void)txt; }
void gsKit_hires_deinit_global(GsGlobal *global) { (void)global; }
void gsKit_deinit_global(GsGlobal *global) { (void)global; }
int gsKit_add_vsync_handler(void *handler) { (void)handler; return 0; }
void gsKit_remv_vsync_handler(int id) { (void)id; }
int gsKit_version(void) { return 3; }

/* Define gColWhite as expected by renderman.c */
const u64 gColWhite = 0xFFFFFFFF00000000ULL;  /* RGBA with alpha in upper bits */
