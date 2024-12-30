/*
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2023-12-08 10:45:26
 * @LastEditTime: 2024-11-11 14:10:11
 * @License: GPL 3.0
 */

#ifndef WIDGET_INIT_H
#define WIDGET_INIT_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "gui_guider.h"

__attribute__((unused)) void kb_event_cb(lv_event_t *e);
__attribute__((unused)) void ta_event_cb(lv_event_t *e);
#if LV_USE_ANALOGCLOCK != 0
void clock_count(int *hour, int *min, int *sec);
#endif



#ifdef __cplusplus
}
#endif
#endif
