/**
  ******************************************************************************
  * @file    APP_LowPower.h
  * @brief   主控低功耗应用层头文件。
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
#ifndef __APP_LOWPOWER_H__
#define __APP_LOWPOWER_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
int32_t APP_LowPower_Init(void);
int32_t APP_LowPower_EarlyWakeConfirm(void);
void APP_LowPower_Task(uint16_t dT_ms);

#ifdef __cplusplus
}
#endif
#endif   /* __APP_LOWPOWER_H__ */
