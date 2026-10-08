/**
  ******************************************************************************
  * @file    Device_BMP580.h
  * @brief   BMP580驱动层头文件。
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
#ifndef __DEVICE_BMP580_H__
#define __DEVICE_BMP580_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "PlatformSPI.h"
/* Private defines -----------------------------------------------------------*/
#define BMP580_IDENHTIFICATION_ID_REG    0x01
#define BMP580_REVISION_ID_REG 			 0x02
#define BMP580_STATUS_REG        		 0x28
#define BMP580_INT_STATUS_REG		     0x27
#define BMP580_INT_SOURCE_REG    		 0x15
#define BMP580_OSR_CONFIG_REG    		 0x36
#define BMP580_ODR_CONFIG_REG    		 0x37
#define BMP580_TEMP_DATA_REG  			 0x1D

#define BMP580_ODR_1HZ           		 0x1C
#define BMP580_ODR_5HZ           		 0x18
#define BMP580_ODR_10HZ          		 0x17
#define BMP580_ODR_20HZ          		 0x15
#define BMP580_ODR_25HZ          		 0x14
#define BMP580_ODR_50HZ          		 0x0F
#define BMP580_ODR_120HZ        		 0x08
#define BMP580_ODR_240HZ        		 0x00


#define BMP580_IDENHTIFICATION_ID        0x50
#define BMP580_REVISION_ID               0x32


#define BMP580_ODR               BMP580_ODR_10HZ
/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
int32_t Device_BMP580_Init(void);
/* 查询数据就绪标志。 */
int32_t Device_BMP580_Data_Ready(uint8_t *value);
/* 读取原始温度和压力字节。 */
int32_t Device_BMP580_Read(uint8_t *buf);
#ifdef __cplusplus
}
#endif
#endif   /* __DEVICE_BMP580_H__ */
