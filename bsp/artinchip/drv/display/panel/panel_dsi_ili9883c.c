/*
 * Copyright (c) 2026, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "panel_com.h"
#include "panel_dsi.h"

#define PANEL_RESET "PA.5"
/* Set to 1 only for panel bring-up diagnostics. */
#define ILI9883C_ENABLE_BIST 0

static struct gpio_desc reset_gpio;

static void panel_gpio_init(void)
{
    panel_get_gpio(&reset_gpio, PANEL_RESET);

    panel_gpio_set_value(&reset_gpio, 1);
    aic_delay_ms(10);
    panel_gpio_set_value(&reset_gpio, 0);
    aic_delay_ms(20);
    panel_gpio_set_value(&reset_gpio, 1);
    aic_delay_ms(150);
}

static int panel_enable(struct aic_panel *panel)
{
    int ret;

    panel_gpio_init();
    panel_di_enable(panel, 0);
    panel_dsi_send_perpare(panel);

    /* UE059HD-AK40-A003B, ILI9883C, 720x1440, MIPI DSI 4 lane. */
    pr_info("ILI9883C init: generic cmd, lane=4, ln_assign=0x3210, dc_inv=1, ln_polrs=0xF, madctl=0x00, colmod=vendor-default, bist=%d\n",
            ILI9883C_ENABLE_BIST);
    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x01);
    panel_dsi_generic_send_seq(panel, 0xC1, 0x00);
    panel_dsi_generic_send_seq(panel, 0x00, 0x4E);
    panel_dsi_generic_send_seq(panel, 0x01, 0x35);
    panel_dsi_generic_send_seq(panel, 0x02, 0x00);
    panel_dsi_generic_send_seq(panel, 0x03, 0x00);
    panel_dsi_generic_send_seq(panel, 0x04, 0xCA);
    panel_dsi_generic_send_seq(panel, 0x05, 0x15);
    panel_dsi_generic_send_seq(panel, 0x06, 0x00);
    panel_dsi_generic_send_seq(panel, 0x07, 0x00);
    panel_dsi_generic_send_seq(panel, 0x08, 0x89);
    panel_dsi_generic_send_seq(panel, 0x09, 0x02);
    panel_dsi_generic_send_seq(panel, 0x0A, 0xB4);
    panel_dsi_generic_send_seq(panel, 0x0B, 0x00);
    panel_dsi_generic_send_seq(panel, 0x0C, 0x53);
    panel_dsi_generic_send_seq(panel, 0x0D, 0x53);
    panel_dsi_generic_send_seq(panel, 0x0E, 0x00);
    panel_dsi_generic_send_seq(panel, 0x0F, 0x00);
    panel_dsi_generic_send_seq(panel, 0x16, 0x89);
    panel_dsi_generic_send_seq(panel, 0x17, 0x02);
    panel_dsi_generic_send_seq(panel, 0x18, 0x34);
    panel_dsi_generic_send_seq(panel, 0x19, 0x00);
    panel_dsi_generic_send_seq(panel, 0x1A, 0x53);
    panel_dsi_generic_send_seq(panel, 0x1B, 0x53);
    panel_dsi_generic_send_seq(panel, 0x1C, 0x00);
    panel_dsi_generic_send_seq(panel, 0x1D, 0x00);
    panel_dsi_generic_send_seq(panel, 0x24, 0x12);
    panel_dsi_generic_send_seq(panel, 0x25, 0x84);
    panel_dsi_generic_send_seq(panel, 0x28, 0x9F);
    panel_dsi_generic_send_seq(panel, 0x2A, 0x9F);
    panel_dsi_generic_send_seq(panel, 0x29, 0x91);
    panel_dsi_generic_send_seq(panel, 0x2B, 0x91);
    panel_dsi_generic_send_seq(panel, 0x31, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x32, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x33, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x34, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x35, 0x22);
    panel_dsi_generic_send_seq(panel, 0x36, 0x22);
    panel_dsi_generic_send_seq(panel, 0x37, 0x23);
    panel_dsi_generic_send_seq(panel, 0x38, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x39, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x3A, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x3B, 0x20);
    panel_dsi_generic_send_seq(panel, 0x3C, 0x10);
    panel_dsi_generic_send_seq(panel, 0x3D, 0x12);
    panel_dsi_generic_send_seq(panel, 0x3E, 0x14);
    panel_dsi_generic_send_seq(panel, 0x3F, 0x16);
    panel_dsi_generic_send_seq(panel, 0x40, 0x18);
    panel_dsi_generic_send_seq(panel, 0x41, 0x1A);
    panel_dsi_generic_send_seq(panel, 0x42, 0x0C);
    panel_dsi_generic_send_seq(panel, 0x43, 0x0A);
    panel_dsi_generic_send_seq(panel, 0x44, 0x08);
    panel_dsi_generic_send_seq(panel, 0x45, 0x07);
    panel_dsi_generic_send_seq(panel, 0x46, 0x07);
    panel_dsi_generic_send_seq(panel, 0x47, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x48, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x49, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x4A, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x4B, 0x22);
    panel_dsi_generic_send_seq(panel, 0x4C, 0x22);
    panel_dsi_generic_send_seq(panel, 0x4D, 0x23);
    panel_dsi_generic_send_seq(panel, 0x4E, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x4F, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x50, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x51, 0x20);
    panel_dsi_generic_send_seq(panel, 0x52, 0x11);
    panel_dsi_generic_send_seq(panel, 0x53, 0x13);
    panel_dsi_generic_send_seq(panel, 0x54, 0x15);
    panel_dsi_generic_send_seq(panel, 0x55, 0x17);
    panel_dsi_generic_send_seq(panel, 0x56, 0x19);
    panel_dsi_generic_send_seq(panel, 0x57, 0x1B);
    panel_dsi_generic_send_seq(panel, 0x58, 0x0D);
    panel_dsi_generic_send_seq(panel, 0x59, 0x0B);
    panel_dsi_generic_send_seq(panel, 0x5A, 0x09);
    panel_dsi_generic_send_seq(panel, 0x5B, 0x07);
    panel_dsi_generic_send_seq(panel, 0x5C, 0x07);
    panel_dsi_generic_send_seq(panel, 0x61, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x62, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x63, 0x22);
    panel_dsi_generic_send_seq(panel, 0x64, 0x22);
    panel_dsi_generic_send_seq(panel, 0x65, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x66, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x67, 0x23);
    panel_dsi_generic_send_seq(panel, 0x68, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x69, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x6A, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x6B, 0x20);
    panel_dsi_generic_send_seq(panel, 0x6C, 0x1B);
    panel_dsi_generic_send_seq(panel, 0x6D, 0x19);
    panel_dsi_generic_send_seq(panel, 0x6E, 0x17);
    panel_dsi_generic_send_seq(panel, 0x6F, 0x15);
    panel_dsi_generic_send_seq(panel, 0x70, 0x13);
    panel_dsi_generic_send_seq(panel, 0x71, 0x11);
    panel_dsi_generic_send_seq(panel, 0x72, 0x0D);
    panel_dsi_generic_send_seq(panel, 0x73, 0x09);
    panel_dsi_generic_send_seq(panel, 0x74, 0x0B);
    panel_dsi_generic_send_seq(panel, 0x75, 0x07);
    panel_dsi_generic_send_seq(panel, 0x76, 0x07);
    panel_dsi_generic_send_seq(panel, 0x77, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x78, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x79, 0x22);
    panel_dsi_generic_send_seq(panel, 0x7A, 0x22);
    panel_dsi_generic_send_seq(panel, 0x7B, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x7C, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x7D, 0x23);
    panel_dsi_generic_send_seq(panel, 0x7E, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x7F, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x80, 0x2A);
    panel_dsi_generic_send_seq(panel, 0x81, 0x20);
    panel_dsi_generic_send_seq(panel, 0x82, 0x1A);
    panel_dsi_generic_send_seq(panel, 0x83, 0x18);
    panel_dsi_generic_send_seq(panel, 0x84, 0x16);
    panel_dsi_generic_send_seq(panel, 0x85, 0x14);
    panel_dsi_generic_send_seq(panel, 0x86, 0x12);
    panel_dsi_generic_send_seq(panel, 0x87, 0x10);
    panel_dsi_generic_send_seq(panel, 0x88, 0x0C);
    panel_dsi_generic_send_seq(panel, 0x89, 0x08);
    panel_dsi_generic_send_seq(panel, 0x8A, 0x0A);
    panel_dsi_generic_send_seq(panel, 0x8B, 0x07);
    panel_dsi_generic_send_seq(panel, 0x8C, 0x07);
    panel_dsi_generic_send_seq(panel, 0xB0, 0x44);
    panel_dsi_generic_send_seq(panel, 0xB1, 0x44);
    panel_dsi_generic_send_seq(panel, 0xE6, 0x22);
    panel_dsi_generic_send_seq(panel, 0xE7, 0x54);
    panel_dsi_generic_send_seq(panel, 0xBA, 0x04);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x02);
    panel_dsi_generic_send_seq(panel, 0x06, 0xAC);
    panel_dsi_generic_send_seq(panel, 0x0A, 0x1B);
    panel_dsi_generic_send_seq(panel, 0x0C, 0x00);
    panel_dsi_generic_send_seq(panel, 0x0D, 0x2C);
    panel_dsi_generic_send_seq(panel, 0x0E, 0x32);
    panel_dsi_generic_send_seq(panel, 0x39, 0x11);
    panel_dsi_generic_send_seq(panel, 0x3A, 0x2C);
    panel_dsi_generic_send_seq(panel, 0x3B, 0x32);
    panel_dsi_generic_send_seq(panel, 0x3C, 0x4A);
    panel_dsi_generic_send_seq(panel, 0xF0, 0x00);
    panel_dsi_generic_send_seq(panel, 0xF1, 0x36);
    panel_dsi_generic_send_seq(panel, 0x48, 0x01);
    panel_dsi_generic_send_seq(panel, 0x44, 0x68);
#if ILI9883C_ENABLE_BIST
    panel_dsi_generic_send_seq(panel, 0x3F, 0x01);
#endif

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x03);
    panel_dsi_generic_send_seq(panel, 0x20, 0x01);
    panel_dsi_generic_send_seq(panel, 0x22, 0xFA);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x05);
    panel_dsi_generic_send_seq(panel, 0x03, 0x00);
    panel_dsi_generic_send_seq(panel, 0x04, 0xB8);
    panel_dsi_generic_send_seq(panel, 0x69, 0x97);
    panel_dsi_generic_send_seq(panel, 0x6A, 0xAD);
    panel_dsi_generic_send_seq(panel, 0x6D, 0x8D);
    panel_dsi_generic_send_seq(panel, 0x73, 0x93);
    panel_dsi_generic_send_seq(panel, 0x79, 0x8D);
    panel_dsi_generic_send_seq(panel, 0x7F, 0x7F);
    panel_dsi_generic_send_seq(panel, 0x68, 0x3E);
    panel_dsi_generic_send_seq(panel, 0x66, 0x33);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x06);
    panel_dsi_generic_send_seq(panel, 0xD9, 0x1F);
    panel_dsi_generic_send_seq(panel, 0xC0, 0xA0);
    panel_dsi_generic_send_seq(panel, 0xC1, 0x15);
    panel_dsi_generic_send_seq(panel, 0x0A, 0x50);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x08);
    panel_dsi_generic_send_seq(panel, 0xE0, 0x00, 0x24, 0x37, 0x65, 0x95, 0x54,
                           0xD3, 0x06, 0x2E, 0x5E, 0x95, 0x86, 0xC4, 0xF6,
                           0x23, 0xAA, 0x4D, 0x7A, 0xAE, 0xCF, 0xFE, 0xF6,
                           0x18, 0x44, 0x78, 0x3F, 0xA2, 0xD6, 0xEC);
    panel_dsi_generic_send_seq(panel, 0xE1, 0x00, 0x24, 0x37, 0x65, 0x95, 0x54,
                           0xD3, 0x06, 0x2E, 0x5E, 0x95, 0x86, 0xC4, 0xF6,
                           0x23, 0xAA, 0x4D, 0x7A, 0xAE, 0xCF, 0xFE, 0xF6,
                           0x18, 0x44, 0x78, 0x3F, 0xA2, 0xD6, 0xEC);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x0A);
    panel_dsi_generic_send_seq(panel, 0xE0, 0x01);
    panel_dsi_generic_send_seq(panel, 0xE1, 0x0B);
    panel_dsi_generic_send_seq(panel, 0xE2, 0x01);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x0B);
    panel_dsi_generic_send_seq(panel, 0x9A, 0xC5);
    panel_dsi_generic_send_seq(panel, 0x9B, 0x6A);
    panel_dsi_generic_send_seq(panel, 0x9C, 0x04);
    panel_dsi_generic_send_seq(panel, 0x9D, 0x04);
    panel_dsi_generic_send_seq(panel, 0x9E, 0x87);
    panel_dsi_generic_send_seq(panel, 0x9F, 0x87);
    panel_dsi_generic_send_seq(panel, 0xAA, 0x22);
    panel_dsi_generic_send_seq(panel, 0xAB, 0xE0);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x0E);
    panel_dsi_generic_send_seq(panel, 0x00, 0xA3);
    panel_dsi_generic_send_seq(panel, 0x02, 0x2C);
    panel_dsi_generic_send_seq(panel, 0x07, 0x21);
    panel_dsi_generic_send_seq(panel, 0x4B, 0x05);
    panel_dsi_generic_send_seq(panel, 0x11, 0x47);
    panel_dsi_generic_send_seq(panel, 0x12, 0x02);
    panel_dsi_generic_send_seq(panel, 0x13, 0x14);
    panel_dsi_generic_send_seq(panel, 0x40, 0x07);
    panel_dsi_generic_send_seq(panel, 0x45, 0x0B);
    panel_dsi_generic_send_seq(panel, 0x46, 0x9B);
    panel_dsi_generic_send_seq(panel, 0x49, 0xB4);
    panel_dsi_generic_send_seq(panel, 0x4D, 0x96);
    panel_dsi_generic_send_seq(panel, 0xC0, 0x01);
    panel_dsi_generic_send_seq(panel, 0xC1, 0x96);
    panel_dsi_generic_send_seq(panel, 0xD0, 0x36);
    panel_dsi_generic_send_seq(panel, 0xD1, 0x01);
    panel_dsi_generic_send_seq(panel, 0xD2, 0x96);
    panel_dsi_generic_send_seq(panel, 0xD3, 0x96);
    panel_dsi_generic_send_seq(panel, 0xD4, 0x96);
    panel_dsi_generic_send_seq(panel, 0xD5, 0x96);
    panel_dsi_generic_send_seq(panel, 0xD6, 0x96);
    panel_dsi_generic_send_seq(panel, 0xD7, 0x96);
    panel_dsi_generic_send_seq(panel, 0xD8, 0x96);
    panel_dsi_generic_send_seq(panel, 0xD9, 0x96);
    panel_dsi_generic_send_seq(panel, 0xDA, 0x44);
    panel_dsi_generic_send_seq(panel, 0xDB, 0x44);
    panel_dsi_generic_send_seq(panel, 0xDC, 0x44);
    panel_dsi_generic_send_seq(panel, 0xDD, 0x44);
    panel_dsi_generic_send_seq(panel, 0x50, 0xC0);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x04);
    panel_dsi_generic_send_seq(panel, 0xBA, 0x81);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x0C);
    panel_dsi_generic_send_seq(panel, 0x00, 0x26);
    panel_dsi_generic_send_seq(panel, 0x01, 0x57);
    panel_dsi_generic_send_seq(panel, 0x02, 0x26);
    panel_dsi_generic_send_seq(panel, 0x03, 0x58);
    panel_dsi_generic_send_seq(panel, 0x04, 0x25);
    panel_dsi_generic_send_seq(panel, 0x05, 0x56);
    panel_dsi_generic_send_seq(panel, 0x06, 0x25);
    panel_dsi_generic_send_seq(panel, 0x07, 0x55);
    panel_dsi_generic_send_seq(panel, 0x08, 0x25);
    panel_dsi_generic_send_seq(panel, 0x09, 0x54);
    panel_dsi_generic_send_seq(panel, 0x0A, 0x26);
    panel_dsi_generic_send_seq(panel, 0x0B, 0x59);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x04);
    panel_dsi_generic_send_seq(panel, 0xBA, 0x01);

    panel_dsi_generic_send_seq(panel, 0xFF, 0x98, 0x83, 0x00);
    panel_dsi_generic_send_seq(panel, 0x35, 0x00);
    panel_dsi_generic_send_seq(panel, 0x36, 0x00);

    ret = panel_dsi_dcs_exit_sleep_mode(panel);
    if (ret < 0) {
        pr_err("Failed to exit sleep mode: %d\n", ret);
        return ret;
    }
    aic_delay_ms(600);

    ret = panel_dsi_dcs_set_display_on(panel);
    if (ret < 0) {
        pr_err("Failed to set display on: %d\n", ret);
        return ret;
    }
    aic_delay_ms(100);

    panel_dsi_setup_realmode(panel);
    panel_de_timing_enable(panel, 0);
    panel_backlight_enable(panel, 0);

    return 0;
}

static struct aic_panel_funcs panel_funcs = {
    .disable = panel_default_disable,
    .unprepare = panel_default_unprepare,
    .prepare = panel_default_prepare,
    .enable = panel_enable,
    .register_callback = panel_register_callback,
};

static struct display_timing ili9883c_timing = {
    .pixelclock = 75000000,
    .hactive = 720,
    .hfront_porch = 40,
    .hback_porch = 40,
    .hsync_len = 12,
    .vactive = 1440,
    .vfront_porch = 50,
    .vback_porch = 44,
    .vsync_len = 4,
};

static struct panel_dsi dsi = {
    .mode = DSI_MOD_VID_BURST,
    .format = DSI_FMT_RGB888,
    .lane_num = 4,
    .ln_assign = 0x3210,
    .dc_inv = 1,
    .ln_polrs = 0xF,
};

struct aic_panel dsi_ili9883c = {
    .name = "panel-ili9883c",
    .timings = &ili9883c_timing,
    .funcs = &panel_funcs,
    .dsi = &dsi,
    .connector_type = AIC_MIPI_COM,
};
