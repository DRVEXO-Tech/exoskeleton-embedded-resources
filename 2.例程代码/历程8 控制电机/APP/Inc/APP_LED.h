/**
  ******************************************************************************
  * @file    APP_LED.h
  * @brief   LED应用层头文件。
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
#ifndef __APP_LED_H__
#define __APP_LED_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Device_IO_Control.h"
/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    APP_LED_MODE_OFF = 0,
    APP_LED_MODE_ON = 1,
    APP_LED_MODE_BLINK = 2
} APP_LED_Mode_en;

/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
int32_t APP_LED_Mode_Set(APP_LED_Mode_en mode);
void APP_LED_TASK(uint16_t dT_ms);


#ifdef __cplusplus
}
#endif
#endif   /* __APP_LED_H__ */
