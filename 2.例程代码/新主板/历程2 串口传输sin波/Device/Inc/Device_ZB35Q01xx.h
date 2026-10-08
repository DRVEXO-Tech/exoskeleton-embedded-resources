/**
  ******************************************************************************
  * @file    Device_ZB35Q01xx.h
  * @brief   ZB35Q01xx驱动层头文件。
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
#ifndef __DEVICE_ZB35Q01XX_H__
#define __DEVICE_ZB35Q01XX_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "PlatformSPI.h"
/* Private defines -----------------------------------------------------------*/

#define ZB35Q01XX_CMD_READ_ID  		0x9F
#define ZB35Q01XX_CMD_RESET         0xFF
#define ZB35Q01XX_CMD_PLOAD         0x02
#define	ZB35Q01XX_CMD_WRITE_ENABLE	0x06




#define ZB35Q01XX_MFR_ID       		0x5E
#define ZB35Q01XX_DEVICE_ID    		0xC1


/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
int32_t Device_ZB35Q01_Init(void);
int32_t Device_ZB35Q01_Unlock(void);
int32_t Device_ZB35Q01_EraseBlock(uint16_t block);
int32_t Device_ZB35Q01_ReadPage(uint32_t page, uint16_t offset,uint8_t *data, uint16_t len);
int32_t Device_ZB35Q01_WritePage(uint32_t page, uint16_t offset,const uint8_t *data, uint16_t len);
#ifdef __cplusplus
}
#endif
#endif   /* __DEVICE_ZB35Q01XX_H__ */
