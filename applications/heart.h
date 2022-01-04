/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-17     Rick       the first version
 */
#ifndef APPLICATIONS_HEART_H_
#define APPLICATIONS_HEART_H_

typedef struct
{
    uint16_t freq;
    uint8_t retry;
    uint8_t alive;
    uint8_t received;
    uint8_t test;
    int rssi;
    uint8_t rssi_level;
}rf_info;

void heart_warning(void);

#endif /* APPLICATIONS_HEART_H_ */
