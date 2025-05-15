#ifdef __has_include
#if __has_include("lvgl.h")
#ifndef LV_LVGL_H_INCLUDE_SIMPLE
#define LV_LVGL_H_INCLUDE_SIMPLE
#endif
#endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

extern const lv_image_dsc_t scene_forest_day;
extern const lv_image_dsc_t scene_forest_night;

extern const lv_image_dsc_t scene_meadow_day;
extern const lv_image_dsc_t scene_meadow_night;

extern const lv_image_dsc_t scene_beach_day;
extern const lv_image_dsc_t scene_beach_night;

extern const lv_image_dsc_t scene_shop_day;

// character sprites
extern const lv_image_dsc_t sprite_base;

extern const lv_image_dsc_t face_angry;
extern const lv_image_dsc_t face_happy;
extern const lv_image_dsc_t face_sad;
extern const lv_image_dsc_t face_sleepy;
