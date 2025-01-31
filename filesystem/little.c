/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-20     Rick       the first version
 */
#include <rtdevice.h>
#include <spi_flash.h>
#include <drv_spi.h>
#include <string.h>
#include <stdlib.h>
#include "fal.h"
#include "dfs_fs.h"
#include "little.h"
#include <dfs_posix.h>
#include "easyflash.h"

#define DBG_TAG "little_fs"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_spi_flash_device_t w25q128;
struct rt_device *mtd_dev = RT_NULL;
#define FS_PARTITION_NAME              "filesystem"

int flash_Init(void)
{
    rt_err_t status;
    extern rt_spi_flash_device_t rt_sfud_flash_probe(const char *spi_flash_dev_name, const char *spi_dev_name);
    rt_hw_spi_device_attach("spi3", "spi30", GPIOA, GPIO_PIN_15);
    w25q128 = rt_sfud_flash_probe("norflash0", "spi30");
    if (RT_NULL == w25q128)
    {
        LOG_E("sfud fail\r\n");
        return RT_ERROR;
    };
    status = fal_init();
    if (status == 0)
    {
        LOG_E("fal_init fail\r\n");
        return RT_ERROR;
    };
    status = easyflash_init();
    if (status != EF_NO_ERR)
    {
        LOG_E("easyflash_init fail\r\n");
        return RT_ERROR;
    };
    mtd_dev = fal_mtd_nor_device_create(FS_PARTITION_NAME);
    if (!mtd_dev)
    {
        LOG_E("Can't create a mtd device on '%s' partition.", FS_PARTITION_NAME);
        return RT_ERROR;
    }
    if (dfs_mount(FS_PARTITION_NAME, "/", "lfs", 0, 0) == 0)
    {
        LOG_I("dfs_mount success\r\n");
    }
    else
    {
        dfs_mkfs("lfs", FS_PARTITION_NAME);
        /* 挂载 littlefs */
        if (dfs_mount("filesystem", "/", "lfs", 0, 0) == 0)
        {
            LOG_I("Filesystem initialized!");
        }
        else
        {
            LOG_E("Failed to initialize filesystem!");
        }
    }
    LOG_I("Storage Init Success\r\n");
    return RT_EOK;
}
uint32_t write_csv(char *buf,uint32_t pos,uint32_t size)
{
    static int fd;
    static uint32_t pos_tmp;
    fd = open("/SPSI_log.csv", O_WRONLY | O_CREAT);
    if (fd>= 0)
    {
        lseek(fd,pos,SEEK_SET);
        write(fd, buf, size);
        pos_tmp = ltell(fd);
        close(fd);
    }
    return pos_tmp;
}
uint32_t file_init(void)
{
    static uint32_t pos_tmp;
    char *buf = rt_malloc(64);
    sprintf(buf,"No: Freq: Valve: PSI: Shake: Tx: Rssi: First: Button:\n");
    unlink("/SPSI_log.csv");
    pos_tmp = write_csv(buf,0,strlen(buf));
    rt_free(buf);
    LOG_I("SPSI_log.csv is created");
    return pos_tmp;
}
MSH_CMD_EXPORT(file_init,file_init);
