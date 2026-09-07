/**
  ******************************************************************************
  * @file    User_main.h
  * @brief   用户二次开发主入口头文件。
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
#ifndef __USER_MAIN_H__
#define __USER_MAIN_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "User_Data.h"

/* Private defines -----------------------------------------------------------*/
/* 首次上电低功耗开关：1表示启动后先进入Standby，0表示直接启动用户程序。 */
#define USER_POWER_ON_STANDBY_ENABLE       1U

/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void User_main(void);


#ifdef __cplusplus
}
#endif
#endif   /* __USER_MAIN_H__ */
