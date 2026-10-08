/**
  ******************************************************************************
  * @file    Platform_SPI.h
  * @brief   SPI 平台接口头文件。
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
#ifndef __PLATFORMSPI_H__
#define __PLATFORMSPI_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private defines -----------------------------------------------------------*/
#define PLATFORM_SPI_READ_MAX_LEN 12U 

/* Exported types ------------------------------------------------------------*/

struct platform_dev_spi_t;
typedef int32_t (*stmdev_spi_write)(struct platform_dev_spi_t *dev,uint8_t reg_addr,uint8_t *data,uint16_t lenth);
typedef int32_t (*stmdev_spi_read)(struct platform_dev_spi_t *dev,uint8_t reg_addr,uint8_t *data,uint16_t lenth);
typedef int32_t (*stmdev_spi_write_cmd)(struct platform_dev_spi_t *dev,const uint8_t *cmd,uint16_t cmd_len,const uint8_t *bufp,uint16_t lenth);
typedef int32_t (*stmdev_spi_read_cmd)(struct platform_dev_spi_t *dev,const uint8_t *cmd,uint16_t cmd_len,uint8_t *bufp,uint16_t lenth);


struct platform_dev_spi_t
{
    void *handle;
	void *CS_GPIO;
	uint16_t CS_Pin;
    stmdev_spi_write write;
    stmdev_spi_read read;
	
	stmdev_spi_write_cmd write_cmd;
	stmdev_spi_read_cmd read_cmd;
};

/* Exported extern variables -------------------------------------------------*/
extern struct platform_dev_spi_t platform_dev_spi_qmi8658;
extern struct platform_dev_spi_t platform_dev_spi_qmi8658_sub;
extern struct platform_dev_spi_t platform_dev_spi_zb35q01;
extern struct platform_dev_spi_t platform_dev_spi_bmp580;
/* Exported functions prototypes ---------------------------------------------*/


#ifdef __cplusplus
}
#endif
#endif   /* __PLATFORMSPI_H__ */
