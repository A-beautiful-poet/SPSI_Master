/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-20     Rick       the first version
 */
#ifndef FILESYSTEM_LITTLE_H_
#define FILESYSTEM_LITTLE_H_
#include "stdint.h"

int flash_Init(void);
uint32_t write_csv(char *buf,uint32_t pos,uint32_t size);
uint32_t file_init(void);

#endif /* FILESYSTEM_LITTLE_H_ */
