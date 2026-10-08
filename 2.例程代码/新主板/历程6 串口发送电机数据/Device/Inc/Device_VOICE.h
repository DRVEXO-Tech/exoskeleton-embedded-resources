/**
  ******************************************************************************
  * @file    Device_VOICE.h
  * @brief   音频驱动平台层头文件
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
#ifndef __DEVICE_VOICE_H__
#define __DEVICE_VOICE_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include "Platform_UART.h"
/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    BUSY = 0x00,
    IDLE = 0x01,
} VOICE_Status_Enum;

/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

VOICE_Status_Enum voicex_is_busy(void);
int32_t voicex_play_voice(uint8_t file_id, uint8_t voice_id);
#ifdef __cplusplus
}
#endif
#endif   /*__DEVICE_VOICE_H__*/
