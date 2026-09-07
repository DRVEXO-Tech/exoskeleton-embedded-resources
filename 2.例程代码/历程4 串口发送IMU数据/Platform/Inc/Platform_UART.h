/**
  ******************************************************************************
  * @file    Platform_UART.h
  * @brief   UART平台层头文件。
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
#ifndef __PLATFORM_UART_H__
#define __PLATFORM_UART_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Platform.h"

/* Private defines -----------------------------------------------------------*/

#define PLATFORM_UART_TX_BUF_SIZE 1024U

/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/

extern stmdev_ctx_t platform_uart_voice_dev;
extern stmdev_ctx_t platform_uart_user_dev;
/* Exported functions prototypes ---------------------------------------------*/

uint8_t platform_is_busy(void *ctx);


#ifdef __cplusplus
}
#endif
#endif   /*__PLATFORM_UART_H__*/
