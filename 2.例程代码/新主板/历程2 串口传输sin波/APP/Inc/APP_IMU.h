/**
  ******************************************************************************
  * @file    APP_IMU.h
  * @brief   IMU 应用头文件。
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
#ifndef __APP_IMU_H__
#define __APP_IMU_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Device_QMI8658A.h"
/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/
extern float acc_g[3];
extern float gyro_dps[3];
extern float acc_g_sub[3];
extern float gyro_dps_sub[3];
extern uint8_t Sub_IMU_Flag;
/* Exported functions prototypes ---------------------------------------------*/
int32_t APP_IMU_Init(void);
void APP_IMU_Data(uint16_t dT_ms);
void APP_IMU_Data_sub(uint16_t dT_ms);
#ifdef __cplusplus
}
#endif
#endif   /* __APP_IMU_H__ */
