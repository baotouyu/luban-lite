/*
 * Copyright (c) 2025, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Ning Fang <ning.fang@artinchip.com>
 */

#include <rtconfig.h>
#ifdef KERNEL_RTTHREAD
#include <lvgl.h>
#include <stdbool.h>
#include <rtthread.h>
#include <stdlib.h>
#include <../components/drivers/include/drivers/touch.h>

static lv_indev_state_t last_state = LV_INDEV_STATE_REL;
static rt_int16_t last_x = 0;
static rt_int16_t last_y = 0;
static lv_indev_t *indev_touchpad;
static rt_int16_t touch_cal_dx;
static rt_int16_t touch_cal_dy = 45;
static rt_uint16_t touch_cal_log_left;

static void touch_apply_display_offset(rt_int16_t *x, rt_int16_t *y)
{
    rt_int32_t raw_x = *x;
    rt_int32_t raw_y = *y;

#if defined(LV_DISPLAY_ROTATE_EN) && defined(LV_ROTATE_90)
    raw_x += touch_cal_dy;
    raw_y -= touch_cal_dx;
#elif defined(LV_DISPLAY_ROTATE_EN) && defined(LV_ROTATE_180)
    raw_x -= touch_cal_dx;
    raw_y -= touch_cal_dy;
#elif defined(LV_DISPLAY_ROTATE_EN) && defined(LV_ROTATE_270)
    raw_x -= touch_cal_dy;
    raw_y += touch_cal_dx;
#else
    raw_x += touch_cal_dx;
    raw_y += touch_cal_dy;
#endif

    if (raw_x < 0)
        raw_x = 0;
    else if (raw_x >= AIC_TOUCH_REPORT_X_COORDINATE)
        raw_x = AIC_TOUCH_REPORT_X_COORDINATE - 1;

    if (raw_y < 0)
        raw_y = 0;
    else if (raw_y >= AIC_TOUCH_REPORT_Y_COORDINATE)
        raw_y = AIC_TOUCH_REPORT_Y_COORDINATE - 1;

    *x = (rt_int16_t)raw_x;
    *y = (rt_int16_t)raw_y;
}

static void touch_calc_display_point(rt_int16_t raw_x, rt_int16_t raw_y,
                                     rt_int16_t *disp_x, rt_int16_t *disp_y)
{
    rt_int32_t x = raw_x;
    rt_int32_t y = raw_y;

#if defined(LV_DISPLAY_ROTATE_EN) && defined(LV_ROTATE_90)
    rt_int32_t tmp = y;
    y = x;
    x = AIC_TOUCH_REPORT_Y_COORDINATE - tmp - 1;
#elif defined(LV_DISPLAY_ROTATE_EN) && defined(LV_ROTATE_180)
    x = AIC_TOUCH_REPORT_X_COORDINATE - x - 1;
    y = AIC_TOUCH_REPORT_Y_COORDINATE - y - 1;
#elif defined(LV_DISPLAY_ROTATE_EN) && defined(LV_ROTATE_270)
    rt_int32_t tmp = y;
    x = AIC_TOUCH_REPORT_X_COORDINATE - x - 1;
    y = AIC_TOUCH_REPORT_Y_COORDINATE - y - 1;
    tmp = y;
    y = x;
    x = AIC_TOUCH_REPORT_Y_COORDINATE - tmp - 1;
#endif

    *disp_x = (rt_int16_t)x;
    *disp_y = (rt_int16_t)y;
}

static void input_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = last_state;
}

void aic_touch_inputevent_cb(rt_int16_t x, rt_int16_t y, rt_uint8_t state)
{
#ifdef AIC_BT_BT8858A
	extern int bt_hid_set_touch_event(int, unsigned short, unsigned short);
	bt_hid_set_touch_event(state, y, x);
#endif
    switch (state)
    {
    case RT_TOUCH_EVENT_UP:
        last_state = LV_INDEV_STATE_RELEASED;
        break;
    case RT_TOUCH_EVENT_MOVE:
    case RT_TOUCH_EVENT_DOWN:
    {
        rt_int16_t in_x = x;
        rt_int16_t in_y = y;

        touch_apply_display_offset(&x, &y);
        if (touch_cal_log_left > 0) {
            rt_int16_t disp_x = 0;
            rt_int16_t disp_y = 0;

            touch_calc_display_point(x, y, &disp_x, &disp_y);
            rt_kprintf("tp_cal: raw=(%d,%d) adj_raw=(%d,%d) disp=(%d,%d) event=%d\n",
                       in_x, in_y, x, y, disp_x, disp_y, state);
            touch_cal_log_left--;
        }

        last_x = x;
        last_y = y;
        last_state = LV_INDEV_STATE_PRESSED;
        break;
    }
#ifdef AIC_MONKEY_TEST
    case RT_TOUCH_MONKEY_TEST:
        last_x = x;
        last_y = y;
        last_state = LV_INDEV_STATE_PRESSED;
        break;
#endif
    }
}

#ifdef RT_USING_FINSH
static void touch_cal_cmd(int argc, char **argv)
{
    if (argc >= 3) {
        touch_cal_dx = (rt_int16_t)strtol(argv[1], RT_NULL, 0);
        touch_cal_dy = (rt_int16_t)strtol(argv[2], RT_NULL, 0);
    }

    if (argc >= 4)
        touch_cal_log_left = (rt_uint16_t)strtoul(argv[3], RT_NULL, 0);

    rt_kprintf("tp_cal: display offset dx=%d dy=%d log_left=%u\n",
               touch_cal_dx, touch_cal_dy, touch_cal_log_left);
    rt_kprintf("tp_cal: usage: tp_cal <display_dx> <display_dy> [log_count]\n");
}
MSH_CMD_EXPORT_ALIAS(touch_cal_cmd, tp_cal, set touch display offset);
#endif

void lv_port_indev_init(void)
{
    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, input_read);
#ifdef AIC_USING_ENCODER
    extern lv_indev_t *lv_aic_encoder_create(void);
    lv_indev_t * encoder = lv_aic_encoder_create();
    if (encoder == NULL)
        return;

    lv_group_set_default(lv_group_create());
    lv_indev_set_group(encoder, lv_group_get_default());
#endif
#ifdef AIC_USE_LV_USB_MOUSE
    extern void lv_aic_mouse_hotplug_init(void);
    lv_aic_mouse_hotplug_init();
#endif
}
#endif
