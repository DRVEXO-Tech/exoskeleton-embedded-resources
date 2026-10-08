/**
  ******************************************************************************
  * @file    APP_LPS27HHTW.h
  * @brief   LPS27HHTW的APP层头文件。
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
#ifndef __APP_LPS27HHTW_H__
#define __APP_LPS27HHTW_H__
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Device_LPS27HHTW.h"
/* Private defines -----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported extern variables -------------------------------------------------*/

extern uint8_t g_baro_drdy_flag ;
extern float temperature_degC_LPS27;
extern float altitude_m;
extern uint8_t barometer_altitude_ready;
extern float_t pressure_hPa;
/* Exported functions prototypes ---------------------------------------------*/
int16_t LPS27HHTx_Init(void);
void LPS27HHTW_BaselineTrackingSet(uint8_t enable);
void LPS27HHTW_Task(uint16_t dT_ms);



#ifdef __cplusplus
}
#endif
#endif   /* __APP_LPS27HHTW_H__ */

