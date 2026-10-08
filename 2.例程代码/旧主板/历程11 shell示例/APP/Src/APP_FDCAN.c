/**
  ******************************************************************************
  * @file           : APP_FDCAN.c
  * @brief          : FDCAN APP层文件
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 DRVEXO.
  * All rights reserved.
  *
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "APP_FDCAN.h"
#include "User_Data.h"

/* Define ------------------------------------------------------------------*/
/* Variable ------------------------------------------------------------------*/
static volatile uint8_t g_app_fdcan_heartbeat_enable = 1U;
Battery_SendAlways_u app_fdcan_battery_data;
Motor_SendAlways_u app_fdcan_left_motor_data;
Motor_SendAlways_u app_fdcan_right_motor_data;

/* Function ------------------------------------------------------------------*/
/**
* @brief 初始化FDCAN APP层
* @param 无
* @return 初始化结果
*/
int32_t APP_FDCAN_Init(void)
{
    int32_t ret;

    ret = fdcan_init();
    return ret;
}

/**
* @brief 发送电机控制命令
* @param id 标准帧ID
* @param data 数据指针
* @param len 数据长度
* @return 发送结果
*/
int32_t APP_FDCAN_SendMotorControl(uint16_t id, const uint8_t *data, uint8_t len)
{
    if((id>0x7FE)||(data == 0)||(len==0))
        return -LY_ERR_INVALID;
    return fdcan_write(id,data,len);
}

void APP_FDCAN_SetHeartbeatEnable(uint8_t enable)
{
    g_app_fdcan_heartbeat_enable = (enable != 0U) ? 1U : 0U;
}

/**
* @brief FDCAN APP层周期任务
* @param dT_ms 任务周期
* @return 无
*/
void APP_FDCAN_Task(uint16_t dT_ms)
{
    static uint32_t g_heartbeat_elapsed_ms = 0;
    Core_SendAlways_u core_send_always;
    int32_t battery_ret;
    uint16_t heartbeat_period_ms;

    (void)fdcan_recover_busoff();

    battery_ret = fdcan_get_battery_data(&app_fdcan_battery_data);
    if(battery_ret == LY_OK)
    {
        User_Battery_Level_Update();
    }
    else if(battery_ret < 0)
    {
        sys_err.fdcan_battery_data_err_cnt++;
    }
    if(fdcan_get_left_motor_data(&app_fdcan_left_motor_data) != LY_OK)
    {
        sys_err.fdcan_left_motor_data_err_cnt++;
    }
    if(fdcan_get_right_motor_data(&app_fdcan_right_motor_data) != LY_OK)
    {
        sys_err.fdcan_right_motor_data_err_cnt++;
    }
    User_Motor_Data_Update();

    if(g_app_fdcan_heartbeat_enable == 0U)
    {
        g_heartbeat_elapsed_ms = 0;
        return;
    }

    /* 空总线使用低频发现心跳，节点应答后恢复正常心跳周期。 */
    if((sys_ready.Bat_Ready == 0U) &&
       (sys_ready.Left_Motor_Ready == 0U) &&
       (sys_ready.Right_Motor_Ready == 0U))
    {
        heartbeat_period_ms = APP_FDCAN_DISCOVERY_HEARTBEAT_PERIOD_MS;
    }
    else
    {
        heartbeat_period_ms = APP_FDCAN_HEARTBEAT_PERIOD_MS;
    }

    g_heartbeat_elapsed_ms +=dT_ms;
    if(g_heartbeat_elapsed_ms >= heartbeat_period_ms)
    {
        g_heartbeat_elapsed_ms = 0;
        core_send_always.data.time = HAL_GetTick();
        if(fdcan_write(FDCANX_CORE_ID,core_send_always.buf,sizeof(core_send_always.buf)) != LY_OK)
        {
            sys_err.fdcan_beat_err_cnt++;
        }
    }

}


