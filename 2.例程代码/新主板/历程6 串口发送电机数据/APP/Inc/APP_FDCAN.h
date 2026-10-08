/**
  ******************************************************************************
  * @file    APP_FDCAN.h
  * @brief   FDCAN的APP层头文件。
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
#ifndef __APP_FDCAN_H__
#define __APP_FDCAN_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Device_FDCAN.h"

/* Private defines ------------------#define APP_FDCAN_HEARTBEAT_PERIOD_MS      5U-----------------------------------------*/
#define APP_FDCAN_HEARTBEAT_PERIOD_MS      5U
#define APP_FDCAN_DISCOVERY_HEARTBEAT_PERIOD_MS  100U

/* Exported extern variables -------------------------------------------------*/
extern Battery_SendAlways_u app_fdcan_battery_data;
extern Motor_SendAlways_u app_fdcan_left_motor_data;
extern Motor_SendAlways_u app_fdcan_right_motor_data;

/* Exported functions prototypes ---------------------------------------------*/
int32_t APP_FDCAN_Init(void);
void APP_FDCAN_Task(uint16_t dT_ms);
int32_t APP_FDCAN_SendMotorControl(uint16_t id, const uint8_t *data, uint8_t len);
void APP_FDCAN_SetHeartbeatEnable(uint8_t enable);

#ifdef __cplusplus
}
#endif
#endif   /* __APP_FDCAN_H__ */
