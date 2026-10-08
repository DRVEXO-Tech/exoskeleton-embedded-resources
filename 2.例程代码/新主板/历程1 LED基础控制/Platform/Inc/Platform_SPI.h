/**
  ******************************************************************************
  * @file    Platform_SPI.h
  * @brief   SPI平台层头文件
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
#ifndef __PLATFORM_SPI_H__
#define __PLATFORM_SPI_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Platform.h"

/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/
extern stmdev_ctx_t platform_spi_w25n01_dev;
extern stmdev_ctx_t platform_spi_ism6dso1_dev;
extern stmdev_ctx_t platform_spi_ism6dso2_dev;
extern stmdev_ctx_t platform_spi_lps22df_dev;
/* Exported functions prototypes ---------------------------------------------*/



#ifdef __cplusplus
}
#endif
#endif   /*__PLATFORM_SPI_H__*/
