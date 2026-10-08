/**
  ******************************************************************************
  * @file    APP_Baro.h
  * @brief   气压计应用层头文件。
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
#ifndef __APP_BARO_H__
#define __APP_BARO_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Device_BMP580.h"
/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
struct Baro_Data_t
{
	float pressure_hPa;//气压   单位hPA
	float temperature;//温度    单位℃
	float Absolute_elevation;//绝对海拔  单位m
	float Relative_elevation;//相对海拔  单位m
	volatile uint32_t sample_sequence;//成功采样后递增，数值相同也算新样本
};
/* Exported extern variables -------------------------------------------------*/
extern struct Baro_Data_t Baro_Data;
/* Exported functions prototypes ---------------------------------------------*/

void APP_Baro_Task(uint16_t dT_ms);
	

#ifdef __cplusplus
}
#endif
#endif   /* __APP_BARO_H__ */
