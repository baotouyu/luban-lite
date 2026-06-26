/*
 * Copyright (c) 2026, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ILITEK_H__
#define __ILITEK_H__

#include <aic_hal_gpio.h>
#include <aic_drv_gpio.h>
#include "drivers/touch.h"

#define ILITEK_SLAVE_ADDR                         0x41
#define ILITEK_MAX_TOUCH                          10

#define ILITEK_P5_X_DEMO_PACKET_ID                0x5A
#define ILITEK_P5_X_DEMO_HIGH_RES_PACKET_ID       0x5B
#define ILITEK_P5_X_DEMO_FINGER_PACKET_ID         0x81

#define ILITEK_P5_X_DEMO_PACKET_LEN               43
#define ILITEK_P5_X_DEMO_HIGH_RES_PACKET_LEN      72
#define ILITEK_P5_X_DEMO_81_PACKET_LEN            90
#define ILITEK_P5_X_DEMO_PACKET_INFO_LEN          3

#endif
