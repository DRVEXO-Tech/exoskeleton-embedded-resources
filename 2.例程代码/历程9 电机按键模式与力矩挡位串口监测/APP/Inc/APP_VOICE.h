/**
  ******************************************************************************
  * @file    APP_VOICE.h
  * @brief   音频的APP层头文件。
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
#ifndef __APP_VOICE_H__
#define __APP_VOICE_H__

/* Includes ------------------------------------------------------------------*/
#include "Device_VOICE.h"
/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/


/* Exported extern variables -------------------------------------------------*/


/* Exported functions prototypes ---------------------------------------------*/
void APP_Voice_Task(uint16_t dT_ms);
int32_t app_voice_enqueue(uint8_t voice_id);
int32_t APP_Voice_PlayBatteryLevel(uint8_t battery_level);
#ifdef __cplusplus
}
#endif
#endif   /* __APP_VOICE_H__ */

