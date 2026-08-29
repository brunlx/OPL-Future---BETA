#ifndef __ANIMATION_H
#define __ANIMATION_H

#include <tamtypes.h>

typedef enum {
    ANIM_LINEAR,
    ANIM_EASE_IN,
    ANIM_EASE_OUT,
    ANIM_EASE_IN_OUT,
    ANIM_BOUNCE,
    ANIM_ELASTIC
} anim_easing_t;

int anim_start(float *target, float start, float end, u32 duration, anim_easing_t easing);
void anim_update_all(void);
void anim_stop_all(void);

#endif