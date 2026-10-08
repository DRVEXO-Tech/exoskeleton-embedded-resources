/**
  ******************************************************************************
  * @file           : Device_BMP580.c
  * @brief          : BMP580驱动层文件
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 DRVEXO.
  * All rights reserved.
  *
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "Device_BMP580.h"

/* Define ------------------------------------------------------------------*/
#define Delay_ms(x) HAL_Delay(x)
/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/

int32_t Device_BMP580_ReadGeneral(void)
{
	int32_t ret;
    uint8_t identification_id;
	uint8_t Revision_ID;
	ret = platform_dev_spi_bmp580.read(&platform_dev_spi_bmp580,BMP580_IDENHTIFICATION_ID_REG,&identification_id, 1);
    if (ret != RET_OK)
		return ret;
    if (identification_id != BMP580_IDENHTIFICATION_ID)
		return -RET_ERROR;
	ret = platform_dev_spi_bmp580.read(&platform_dev_spi_bmp580,BMP580_REVISION_ID_REG,&Revision_ID, 1);
    if (ret != RET_OK)
		return ret;
    if (Revision_ID != BMP580_REVISION_ID)
		return -RET_ERROR;
    return RET_OK;
}
int32_t Device_BMP580_Reset(void)
{
	return RET_OK;
}

int32_t Device_BMP580_WaitReady(uint32_t Time_Out)
{
    uint8_t status = 0;
    int32_t ret;
    for (uint32_t i = 0; i < Time_Out; i++)
    {
        ret = platform_dev_spi_bmp580.read(&platform_dev_spi_bmp580, BMP580_STATUS_REG, &status, 1);
        if (ret != RET_OK)
            return ret;
        if (status & 0x04U)  // NVM错误 
            return -RET_ERROR;
        if (status & 0x02U)  // NVM就绪 
            return RET_OK;
        Delay_ms(1);
    }
    return -RET_TIMEOUT;
}



int32_t Device_BMP580_Init(void)
{
    struct platform_dev_spi_t *dev = &platform_dev_spi_bmp580;
    int32_t ret;
    uint8_t value;
    ret = dev->read(dev, BMP580_IDENHTIFICATION_ID_REG, &value, 1);
    if (ret != RET_OK) return ret;
    Delay_ms(1);
    ret = Device_BMP580_ReadGeneral();
    if (ret != RET_OK) return ret;
    ret = Device_BMP580_WaitReady(50);
    if (ret != RET_OK) return ret;
    ret = dev->read(dev, BMP580_INT_STATUS_REG, &value, 1);
    if (ret != RET_OK) return ret;
    value = (uint8_t)(0x80U | (BMP580_ODR << 2));
    ret = dev->write(dev, BMP580_ODR_CONFIG_REG, &value, 1);
    if (ret != RET_OK) return ret;
    Delay_ms(5);
    value = 0x40U; 
    ret = dev->write(dev, BMP580_OSR_CONFIG_REG, &value, 1);
    if (ret != RET_OK) return ret;
    value = 0x01U; 
    ret = dev->write(dev, BMP580_INT_SOURCE_REG, &value, 1);
    if (ret != RET_OK) return ret;
    value = (uint8_t)(0x80U | (BMP580_ODR << 2) | 0x01U);
    return dev->write(dev, BMP580_ODR_CONFIG_REG, &value, 1);
}


int32_t Device_BMP580_Data_Ready(uint8_t *value)
{
    uint8_t status = 0;
    int32_t ret;
    ret = platform_dev_spi_bmp580.read(&platform_dev_spi_bmp580, BMP580_INT_STATUS_REG, &status, 1);
    if (ret != RET_OK)
        return ret;
    *value = status & 0x01U;
    return RET_OK;
}

int32_t Device_BMP580_Read(uint8_t *buf)
{
	return platform_dev_spi_bmp580.read(&platform_dev_spi_bmp580, BMP580_TEMP_DATA_REG, buf, 6);
}

