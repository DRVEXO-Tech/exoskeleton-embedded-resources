/**
  ******************************************************************************
  * @file    Platform_GPIO.h
  * @brief   GPIO平台层头文件
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
#ifndef __PLATFORM_GPIO_H__
#define __PLATFORM_GPIO_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Platform.h"

/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/
extern stmdev_ctx_t platform_gpio_flash_wp_dev;
extern stmdev_ctx_t platform_gpio_flash_hold_dev;
extern stmdev_ctx_t platform_gpio_cs1_dev;
extern stmdev_ctx_t platform_gpio_cs2_dev;
extern stmdev_ctx_t platform_gpio_cs3_dev;
extern stmdev_ctx_t platform_gpio_cs4_dev;
extern stmdev_ctx_t platform_gpio_VM_dev;
extern stmdev_ctx_t platform_gpio_5V_dev;
extern stmdev_ctx_t platform_gpio_3V3_dev;
extern stmdev_ctx_t platform_gpio_fdcan_stb_dev;
extern stmdev_ctx_t platform_gpio_fdcan2_stb_dev;
extern stmdev_ctx_t platform_gpio_voice_busy_dev;
extern stmdev_ctx_t platform_gpio_led_dev;
/* Exported functions prototypes ---------------------------------------------*/
int32_t Platform_WakeupKeys_Init(void);
int32_t Platform_WakeupKeys_Read(uint8_t *pressed_mask);


#ifdef __cplusplus
}
#endif
#endif   /*__PLATFORM_GPIO_H__*/
