/**
  ******************************************************************************
  * @file    Platform_FDCAN.h
  * @brief   FDCAN平台层头文件
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
#ifndef __PLATFORM_FDCAN_H__
#define __PLATFORM_FDCAN_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Platform.h"

/* Private defines -----------------------------------------------------------*/
#define PLATFORM_FDCAN_DATA_MAX_LEN        64U
#define PLATFORM_FDCAN_BAT_BUF_LEN         48U
#define PLATFORM_FDCAN_MOTOR_BUF_LEN       48U
/* Exported types ------------------------------------------------------------*/
struct platform_priv_data_t
{
    uint8_t receive_data[PLATFORM_FDCAN_DATA_MAX_LEN];
    uint16_t rx_len;
    uint32_t id;
};

struct platform_extern_data_t
{
    uint8_t Batter_buf[PLATFORM_FDCAN_BAT_BUF_LEN];                              /* 电池数据缓存 */

    uint8_t Leftmotor_buf[PLATFORM_FDCAN_MOTOR_BUF_LEN];                         /* 左电机周期数据缓存 */
    uint8_t Rightmotor_buf[PLATFORM_FDCAN_MOTOR_BUF_LEN];                        /* 右电机周期数据缓存 */
    uint32_t Leftmotor_last_tick_ms;                                              /* 左电机上次周期数据接收时间 */
    uint32_t Rightmotor_last_tick_ms;                                             /* 右电机上次周期数据接收时间 */
    uint32_t Leftmotor_dt_ms;                                                     /* 左电机周期数据接收间隔 */
    uint32_t Rightmotor_dt_ms;                                                    /* 右电机周期数据接收间隔 */

    uint8_t Leftmotor_receive_one_data;                                          /* 左电机普通单字节回复数据 */
    uint8_t Rightmotor_receive_one_data;                                         /* 右电机普通单字节回复数据 */
    volatile uint8_t Leftmotor_receive_flag;                                     /* 左电机普通接收完成标志 */
    volatile uint8_t Rightmotor_receive_flag;                                    /* 右电机普通接收完成标志 */

};

/* Exported extern variables -------------------------------------------------*/
extern struct platform_priv_data_t platform_priv_data;
extern struct platform_extern_data_t platform_extern_data;
extern stmdev_ctx_t platform_fdcan_dev;

int32_t Platform_FDCAN_RecoverBusOff(void);

#ifdef __cplusplus
}
#endif
#endif   /* __PLATFORM_FDCAN_H__ */
