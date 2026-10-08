/**
  ******************************************************************************
  * @file    Device_IO_Control.h
  * @brief   IO控制设备层头文件。
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
#ifndef __DEVICE_IO_CONTROL_H__
#define __DEVICE_IO_CONTROL_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Platform_GPIO.h"
/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
int32_t Device_EN_VM_Write(uint8_t data);
int32_t Device_EN_5V_Write(uint8_t data);
int32_t Device_EN_3V3_Write(uint8_t data);
int32_t Device_LED_Write(uint8_t data);
int32_t Device_LED_Toggle(void);
int32_t Device_WakeupKeys_Init(void);
int32_t Device_WakeupKeys_Read(uint8_t *pressed_mask);
#ifdef __cplusplus
}
#endif
#endif   /* __DEVICE_IO_CONTROL_H__ */
