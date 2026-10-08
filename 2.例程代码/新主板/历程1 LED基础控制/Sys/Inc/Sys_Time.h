/**
  ******************************************************************************
  * @file    Sys_Time.h
  * @brief   系统计时头文件。
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
#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
uint32_t GetTick_ms(void);
void Sys_IncTick_ms(void);
int32_t Sys_Time_Init_us(void);
uint32_t Sys_Time_Start_us(void);
uint32_t Sys_Time_End_us(uint32_t start_cycle);
uint32_t Sys_Delay_us(uint16_t us);
#ifdef __cplusplus
}
#endif
#endif   /* __SYS_TIME_H__ */
