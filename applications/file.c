/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-23     Rick       the first version
 */
#include "rtthread.h"
#include "stdint.h"
#include "string.h"
#include "file.h"
#include "flashwork.h"
#include "finsh.h"
#include "little.h"

#define DBG_TAG "file"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint32_t Global_Nums=0;
uint32_t Global_Pos=0;

rt_mutex_t spsi_read_mutex = RT_NULL;

void ID_Init(void)
{
    spsi_read_mutex = rt_mutex_create("spsi_read_mutex", RT_IPC_FLAG_PRIO);
    if(Flash_Get_Boot()==0)
    {
        Flash_Boot_Change(1);
        Global_Nums = 0;
        Flash_IDNums_Change(0);
        Global_Pos = file_init();
        Flash_Pos_Change(Global_Pos);
    }
    else
    {
        Global_Nums = Flash_Get_IDNums();
        Global_Pos = Flash_Get_Pos();
    }
    LOG_I("Init ID is %d,Init Pos is %d\r\n",Global_Nums,Global_Pos);
}
void File_Output(uint8_t select,uint8_t valve,uint8_t psi,uint8_t shake,uint8_t send_num,int rssi,uint8_t first,uint8_t button)
{
    rt_err_t result;
    result = rt_mutex_trytake(spsi_read_mutex);
    if(result == RT_EOK)
    {
        char *buf = rt_malloc(64);
        Global_Nums ++;//序列增加
        sprintf(buf,"%d %d %d %d %d %d %d %d %d\n",Global_Nums,(select>0)?433:4068,valve,psi,shake,send_num,rssi,first,button);
        if(Global_Nums%200000==0)
        {
            Global_Pos = 54;
        }
        Global_Pos = write_csv(buf,Global_Pos,strlen(buf));
        Flash_IDNums_Change(Global_Nums);
        Flash_Pos_Change(Global_Pos);
        LOG_D("Write to Flash %s\r\n",buf);
        rt_free(buf);
    }
    else if(result == -RT_ETIMEOUT)
    {
        LOG_E("File is reading now,write again later\r\n");
    }
    rt_mutex_release(spsi_read_mutex);
}
void spsiread(void)
{
    rt_err_t result;
    rt_device_t dev;
    result = rt_mutex_trytake(spsi_read_mutex);
    if(result == RT_EOK)
    {
        dev = rt_console_get_device();
        rt_console_set_device("uart2");
        finsh_set_device("uart2");
        rym_upload_file(dev, "/SPSI_log.csv");
        rt_console_set_device("uart1");
        finsh_set_device("uart1");
    }
    else if(result == -RT_ETIMEOUT)
    {
        LOG_E("File is writing now,read again later\r\n");
    }
    rt_mutex_release(spsi_read_mutex);
}
MSH_CMD_EXPORT(spsiread,spsiread);
