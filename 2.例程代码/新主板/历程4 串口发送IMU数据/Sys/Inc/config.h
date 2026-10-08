/**
  ******************************************************************************
  * @file    config.h
  * @brief   配置头文件。
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
#ifndef __CONFIG_H__
#define __CONFIG_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private defines -----------------------------------------------------------*/

#define LOWPOWER_FLAG_DEBUG          0U
#define APP_VERSION_0         0
#define APP_VERSION_1         1
#define APP_VERSION_2         7

#define USRCONFIG_START_ADDR      0x080FE000U
#define USRCONFIG_SIZE            0x2000U
#define USRCONFIG_BANK            FLASH_BANK_2
#define USRCONFIG_SECTOR          FLASH_SECTOR_63

/* Exported types ------------------------------------------------------------*/
/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
int32_t Sys_UsrConfig_Read(void *data, uint32_t length);
int32_t Sys_UsrConfig_Erase(void);
int32_t Sys_UsrConfig_Write(const void *data, uint32_t length);

#ifdef __cplusplus
}
#endif
#endif   /* __CONFIG_H__ */
