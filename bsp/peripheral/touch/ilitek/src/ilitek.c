/*
 * Copyright (c) 2026, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include "ilitek.h"
#include "touch_common.h"

#define DBG_TAG     AIC_TOUCH_PANEL_NAME
#define DBG_LVL     DBG_INFO
#include <rtdbg.h>

static struct rt_i2c_client g_ilitek_client;
static int16_t g_pre_x[ILITEK_MAX_TOUCH];
static int16_t g_pre_y[ILITEK_MAX_TOUCH];
static rt_uint8_t g_touch_down[ILITEK_MAX_TOUCH];
static rt_uint8_t g_packet_logged;
static rt_uint8_t g_coord_logged;

static struct rt_touch_info g_ilitek_info = {
    RT_TOUCH_TYPE_CAPACITANCE,
    RT_TOUCH_VENDOR_UNKNOWN,
    ILITEK_MAX_TOUCH,
    (rt_int32_t)AIC_TOUCH_X_COORDINATE_RANGE,
    (rt_int32_t)AIC_TOUCH_Y_COORDINATE_RANGE,
};

static rt_err_t ilitek_i2c_read(struct rt_i2c_client *dev, rt_uint8_t *data,
                                rt_uint16_t len)
{
    struct rt_i2c_msg msg;

    msg.addr = dev->client_addr;
    msg.flags = RT_I2C_RD;
    msg.buf = data;
    msg.len = len;

    return (rt_i2c_transfer(dev->bus, &msg, 1) == 1) ? RT_EOK : -RT_ERROR;
}

static void ilitek_touch_up(struct rt_touch_data *data, rt_uint8_t id)
{
    if (id >= ILITEK_MAX_TOUCH)
        return;

    if (g_touch_down[id])
        data[id].event = RT_TOUCH_EVENT_UP;
    else
        data[id].event = RT_TOUCH_EVENT_NONE;

    g_touch_down[id] = 0;
    data[id].timestamp = rt_touch_get_ts();
    data[id].x_coordinate = g_pre_x[id] < 0 ? 0 : g_pre_x[id];
    data[id].y_coordinate = g_pre_y[id] < 0 ? 0 : g_pre_y[id];
    data[id].track_id = id;

    g_pre_x[id] = -1;
    g_pre_y[id] = -1;
}

static void ilitek_touch_down(struct rt_touch_data *data, rt_uint8_t id,
                              int16_t x, int16_t y)
{
    if (id >= ILITEK_MAX_TOUCH)
        return;

    data[id].event = g_touch_down[id] ? RT_TOUCH_EVENT_MOVE : RT_TOUCH_EVENT_DOWN;
    g_touch_down[id] = 1;
    data[id].timestamp = rt_touch_get_ts();
    data[id].x_coordinate = x;
    data[id].y_coordinate = y;
    data[id].track_id = id;

    g_pre_x[id] = x;
    g_pre_y[id] = y;
}

static rt_uint8_t ilitek_packet_len(rt_uint8_t packet_id)
{
    switch (packet_id) {
    case ILITEK_P5_X_DEMO_PACKET_ID:
        return ILITEK_P5_X_DEMO_PACKET_LEN;
    case ILITEK_P5_X_DEMO_HIGH_RES_PACKET_ID:
        return ILITEK_P5_X_DEMO_HIGH_RES_PACKET_LEN;
    case ILITEK_P5_X_DEMO_FINGER_PACKET_ID:
        return ILITEK_P5_X_DEMO_81_PACKET_LEN;
    default:
        return ILITEK_P5_X_DEMO_HIGH_RES_PACKET_LEN;
    }
}

static rt_uint8_t ilitek_parse_point(rt_uint8_t *buf, rt_uint8_t packet_id,
                                     rt_uint8_t index, int16_t *x, int16_t *y)
{
    rt_uint8_t off;

    if (packet_id == ILITEK_P5_X_DEMO_PACKET_ID) {
        off = (4 * index) + 1;
        if (buf[off] == 0xFF && buf[off + 1] == 0xFF && buf[off + 2] == 0xFF)
            return 0;

        *x = ((buf[off] & 0xF0) << 4) | buf[off + 1];
        *y = ((buf[off] & 0x0F) << 8) | buf[off + 2];
    } else if (packet_id == ILITEK_P5_X_DEMO_HIGH_RES_PACKET_ID) {
        off = (5 * index) + 1 + ILITEK_P5_X_DEMO_PACKET_INFO_LEN;
        if (buf[off] == 0xFF && buf[off + 1] == 0xFF &&
            buf[off + 2] == 0xFF && buf[off + 3] == 0xFF)
            return 0;

        *x = (buf[off] << 8) | buf[off + 1];
        *y = (buf[off + 2] << 8) | buf[off + 3];
    } else if (packet_id == ILITEK_P5_X_DEMO_FINGER_PACKET_ID) {
        off = (8 * index) + 1 + ILITEK_P5_X_DEMO_PACKET_INFO_LEN;
        if (buf[off] == 0xFF && buf[off + 1] == 0xFF &&
            buf[off + 2] == 0xFF && buf[off + 3] == 0xFF)
            return 0;

        *x = (buf[off] << 8) | buf[off + 1];
        *y = (buf[off + 2] << 8) | buf[off + 3];
    } else {
        return 0;
    }

    return 1;
}

static rt_size_t ilitek_read_point(struct rt_touch_device *touch, void *buf,
                                   rt_size_t read_num)
{
    rt_uint8_t raw[ILITEK_P5_X_DEMO_81_PACKET_LEN] = {0};
    rt_uint8_t packet_id, packet_len;
    rt_uint8_t active[ILITEK_MAX_TOUCH] = {0};
    rt_uint8_t report_num, event_num = 0;
    struct rt_touch_data *data = (struct rt_touch_data *)buf;

    if ((buf == RT_NULL) || (read_num == 0))
        return 0;

    report_num = read_num > ILITEK_MAX_TOUCH ? ILITEK_MAX_TOUCH : read_num;
    rt_memset(buf, 0, sizeof(struct rt_touch_data) * read_num);

    if (ilitek_i2c_read(&g_ilitek_client, raw, ILITEK_P5_X_DEMO_PACKET_LEN) != RT_EOK) {
        LOG_E("read ilitek packet header failed");
        return 0;
    }

    packet_id = raw[0];
    packet_len = ilitek_packet_len(packet_id);
    if (packet_len > ILITEK_P5_X_DEMO_PACKET_LEN) {
        if (ilitek_i2c_read(&g_ilitek_client, raw, packet_len) != RT_EOK) {
            LOG_E("read ilitek packet 0x%02x len %d failed", packet_id, packet_len);
            return 0;
        }
    }

    if (!g_packet_logged) {
        LOG_I("first packet id=0x%02x len=%d data=%02x %02x %02x %02x %02x %02x %02x %02x",
              packet_id, packet_len, raw[0], raw[1], raw[2], raw[3],
              raw[4], raw[5], raw[6], raw[7]);
        g_packet_logged = 1;
    }

    if (packet_id != ILITEK_P5_X_DEMO_PACKET_ID &&
        packet_id != ILITEK_P5_X_DEMO_HIGH_RES_PACKET_ID &&
        packet_id != ILITEK_P5_X_DEMO_FINGER_PACKET_ID) {
        LOG_D("unsupported packet id=0x%02x", packet_id);
        return 0;
    }

    for (rt_uint8_t i = 0; i < report_num; i++) {
        int16_t x = 0, y = 0;
        int16_t raw_x = 0, raw_y = 0;

        if (!ilitek_parse_point(raw, packet_id, i, &x, &y))
            continue;

        if (x >= AIC_TOUCH_X_COORDINATE_RANGE ||
            y >= AIC_TOUCH_Y_COORDINATE_RANGE)
            continue;

        raw_x = x;
        raw_y = y;
        aic_touch_flip(&x, &y);
        aic_touch_rotate(&x, &y);
        aic_touch_scale(&x, &y);
        if (!aic_touch_crop(&x, &y))
            continue;

        if (!g_coord_logged) {
            LOG_I("first touch coord: raw=(%d,%d) final=(%d,%d) range=%dx%d",
                  raw_x, raw_y, x, y,
                  AIC_TOUCH_X_COORDINATE_RANGE,
                  AIC_TOUCH_Y_COORDINATE_RANGE);
            g_coord_logged = 1;
        }

        ilitek_touch_down(data, i, x, y);
        active[i] = 1;
        if (event_num < i + 1)
            event_num = i + 1;
    }

    for (rt_uint8_t i = 0; i < report_num; i++) {
        if (!active[i] && g_touch_down[i]) {
            ilitek_touch_up(data, i);
            if (event_num < i + 1)
                event_num = i + 1;
        }
    }

    return event_num;
}

static rt_err_t ilitek_control(struct rt_touch_device *touch, int cmd, void *arg)
{
    if (cmd == RT_TOUCH_CTRL_GET_INFO) {
        if (arg == RT_NULL)
            return -RT_EINVAL;

        rt_memcpy(arg, &touch->info, sizeof(struct rt_touch_info));
    }

    return RT_EOK;
}

static struct rt_touch_ops g_ilitek_touch_ops = {
    .touch_readpoint = ilitek_read_point,
    .touch_control = ilitek_control,
};

static void ilitek_reset(struct rt_touch_config *cfg)
{
    rt_uint8_t rst_pin = *(rt_uint8_t *)cfg->user_data;

    rt_pin_mode(rst_pin, PIN_MODE_OUTPUT);
    rt_pin_write(rst_pin, PIN_LOW);
    rt_thread_mdelay(20);
    rt_pin_write(rst_pin, PIN_HIGH);
    rt_thread_mdelay(120);

    rt_pin_mode(cfg->irq_pin.pin, PIN_MODE_INPUT);
}

static int ilitek_hw_init(const char *name, struct rt_touch_config *cfg)
{
    struct rt_touch_device *touch_device;

    touch_device = (struct rt_touch_device *)rt_calloc(1, sizeof(struct rt_touch_device));
    if (touch_device == RT_NULL) {
        LOG_E("touch device malloc fail");
        return -RT_ENOMEM;
    }

    for (rt_uint8_t i = 0; i < ILITEK_MAX_TOUCH; i++) {
        g_pre_x[i] = -1;
        g_pre_y[i] = -1;
    }

    ilitek_reset(cfg);

    g_ilitek_client.bus = (struct rt_i2c_bus_device *)rt_device_find(cfg->dev_name);
    if (g_ilitek_client.bus == RT_NULL) {
        LOG_E("Can't find %s device", cfg->dev_name);
        return -RT_ERROR;
    }

    if (rt_device_open((rt_device_t)g_ilitek_client.bus, RT_DEVICE_FLAG_RDWR) != RT_EOK) {
        LOG_E("open %s device failed", cfg->dev_name);
        return -RT_ERROR;
    }

    g_ilitek_client.client_addr = ILITEK_SLAVE_ADDR;

    LOG_I("ILITEK TDDI init: i2c=%s addr=0x%02x rst=%s int=%s range=%dx%d",
          cfg->dev_name, ILITEK_SLAVE_ADDR, AIC_TOUCH_PANEL_RST_PIN,
          AIC_TOUCH_PANEL_INT_PIN, AIC_TOUCH_X_COORDINATE_RANGE,
          AIC_TOUCH_Y_COORDINATE_RANGE);

    touch_device->info = g_ilitek_info;
    rt_memcpy(&touch_device->config, cfg, sizeof(struct rt_touch_config));
    touch_device->ops = &g_ilitek_touch_ops;

    if (RT_EOK != rt_hw_touch_register(touch_device, name, RT_DEVICE_FLAG_INT_RX, RT_NULL)) {
        LOG_E("touch device ilitek init failed");
        return -RT_ERROR;
    }

    LOG_I("touch device ilitek init success");
    return RT_EOK;
}

static int ilitek_gpio_cfg(void)
{
    unsigned int g, p;
    long pin;

    pin = drv_pin_get(AIC_TOUCH_PANEL_RST_PIN);
    g = GPIO_GROUP(pin);
    p = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_output(g, p);

    pin = drv_pin_get(AIC_TOUCH_PANEL_INT_PIN);
    g = GPIO_GROUP(pin);
    p = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_input(g, p);
    hal_gpio_set_irq_mode(g, p, 0);

    return 0;
}

static int rt_hw_ilitek_port(void)
{
    struct rt_touch_config cfg = {0};
    rt_uint8_t rst_pin;

    ilitek_gpio_cfg();

    rst_pin = drv_pin_get(AIC_TOUCH_PANEL_RST_PIN);
    cfg.dev_name = AIC_TOUCH_PANEL_I2C_CHAN;
    cfg.irq_pin.pin = drv_pin_get(AIC_TOUCH_PANEL_INT_PIN);
    cfg.irq_pin.mode = PIN_MODE_INPUT;
    cfg.user_data = &rst_pin;

    ilitek_hw_init(AIC_TOUCH_PANEL_NAME, &cfg);
    return 0;
}

INIT_DEVICE_EXPORT(rt_hw_ilitek_port);
