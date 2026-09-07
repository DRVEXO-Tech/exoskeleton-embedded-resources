/**
  ******************************************************************************
  * @file    Sys.h
  * @brief   系统定义头文件。
  *          
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 DRVEXO.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYS_H__
#define __SYS_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private defines -----------------------------------------------------------*/
#define LY_OK               0  /* 成功 */
#define LY_ERR              1  /* 通用错误 */
#define LY_ERR_INVALID      2  /* 参数错误 */
#define LY_ERR_TIMEOUT      3  /* 超时 */
#define LY_ERR_BUSY         4  /* 设备忙 */
#define LY_NO_DATA          5  /* 无数据 */
#define LY_ERR_IO           6  /* 底层通信失败 */
#define LY_ERR_NOT_READY    7  /* 设备未就绪 */
#define LY_ERR_UNSUPPORTED  8  /* 不支持的操作 */


/* Exported types ------------------------------------------------------------*/
#pragma pack(push, 1)
typedef struct
{
    uint32_t version;
    uint8_t version_buf[3];
}Version_t;
#pragma pack(pop)

struct Firmware_Version_Data_st
{
    Version_t core;
};

struct sys_err_t{
    //总初始化错误
    uint32_t Sys_Init_err_cnt;
    uint32_t FDCAN_Init_err_cnt;//FDCAN初始化失败
    uint32_t IMU_Init_err_cnt;//陀螺仪初始化失败
    uint32_t Barometer_Init_err_cnt;
    uint32_t Flash_Init_err_cnt;
    uint32_t fdcan_beat_err_cnt;//fdcan心跳帧发送错误
    uint32_t fdcan_battery_data_err_cnt;//fdcan电池周期数据读取错误
    uint32_t fdcan_left_motor_data_err_cnt;//fdcan左电机周期数据读取错误
    uint32_t fdcan_right_motor_data_err_cnt;//fdcan右电机周期数据读取错误
    uint32_t LowPower_Init_err_cnt;//低功耗模块初始化或唤醒确认错误
    uint32_t LowPower_Task_Create_err_cnt;//低功耗后台任务创建错误
    uint32_t LowPower_IO_err_cnt;//低功耗按键或电源IO操作错误
    uint32_t LowPower_Standby_err_cnt;//进入Standby后异常返回错误
    uint32_t Voice_Play_err_cnt;//语音播放错误

    
    uint32_t Motor_Enable_err_cnt;
    uint32_t Motor_Disable_err_cnt;
    uint32_t Motor_Mode_Set_err_cnt;//用户层电机模式设置错误
    uint32_t Motor_Control_err_cnt;//用户层电机控制命令错误
    uint32_t Motor_Calibration_Pass_Notify_err_cnt;//电机校准通过通知错误
    uint32_t Core_Error_Status_Send_err_cnt;//主控错误状态发送错误
    uint32_t LED_Task_err_cnt;//LED任务执行错误
    uint32_t User_UART_tx_err_cnt;//用户串口发送错误
};
struct sys_ready_t{
    uint8_t IMU_Ready;
    uint8_t Baro_Ready;
    uint8_t Flash_Ready;
    uint8_t Left_Motor_Ready;
    uint8_t Right_Motor_Ready;
    uint8_t Bat_Ready;
    
    uint8_t all_ready;
};
/* Exported extern variables -------------------------------------------------*/

extern struct sys_err_t sys_err;
extern struct sys_ready_t sys_ready;
extern struct Firmware_Version_Data_st Firmware_Version_data;
/* Exported functions prototypes ---------------------------------------------*/
int Sys_Init(void);



#ifdef __cplusplus
}
#endif
#endif   /* __SYS_H__ */
