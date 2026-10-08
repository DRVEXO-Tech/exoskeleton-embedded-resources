/**
  ******************************************************************************
  * @file           : Platform_SPI.c
  * @brief          : SPI 平台接口文件
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
#include "PlatformSPI.h"

/* Define ------------------------------------------------------------------*/
#define PLATFORM_SPI_TIMEOUT_MS 100
#define SPI_Delay_us Sys_Delay_us

#define SPI_CS_SET(GPIOx, GPIO_Pin) HAL_GPIO_WritePin((GPIOx), (GPIO_Pin), GPIO_PIN_SET)
#define SPI_CS_RESET(GPIOx, GPIO_Pin) HAL_GPIO_WritePin((GPIOx), (GPIO_Pin), GPIO_PIN_RESET)
/* Variable ------------------------------------------------------------------*/
int32_t platform_spi_write(struct platform_dev_spi_t *dev,uint8_t reg_addr,uint8_t *data, uint16_t lenth);
int32_t platform_spi_read(struct platform_dev_spi_t *dev,uint8_t reg_addr,uint8_t *data, uint16_t lenth);
int32_t platform_write_cmd(struct platform_dev_spi_t *dev,const uint8_t *cmd,uint16_t cmd_len,const uint8_t *bufp,uint16_t lenth);
int32_t platform_read_cmd(struct platform_dev_spi_t *dev,const uint8_t *cmd,uint16_t cmd_len,uint8_t *bufp,uint16_t lenth);
int32_t platform_spi_read_mode2(struct platform_dev_spi_t *dev,uint8_t reg_addr,uint8_t *data, uint16_t lenth);
struct platform_dev_spi_t platform_dev_spi_zb35q01 =
{
    .handle = &hspi1,
	.CS_GPIO = SPI1_CS_GPIO_Port,
	.CS_Pin = SPI1_CS_Pin,
    .write_cmd = platform_write_cmd,
    .read_cmd = platform_read_cmd
};
struct platform_dev_spi_t platform_dev_spi_qmi8658 =
{
    .handle = &hspi2,
	.CS_GPIO = SPI2_CS_GPIO_Port,
	.CS_Pin = SPI2_CS_Pin,
    .write = platform_spi_write,
    .read = platform_spi_read
};
struct platform_dev_spi_t platform_dev_spi_qmi8658_sub =
{
    .handle = &hspi3,
	.CS_GPIO = SPI3_CS_GPIO_Port,
	.CS_Pin = SPI3_CS_Pin,
    .write = platform_spi_write,
    .read = platform_spi_read
};

struct platform_dev_spi_t platform_dev_spi_bmp580 =
{
    .handle = &hspi4,
	.CS_GPIO = SPI4_CS_GPIO_Port,
	.CS_Pin = SPI4_CS_Pin,
    .write = platform_spi_write,
    .read = platform_spi_read_mode2
};
/* Function ------------------------------------------------------------------*/


int32_t platform_spi_write(struct platform_dev_spi_t *dev,uint8_t reg_addr,uint8_t *data, uint16_t lenth)
{	
	if(dev->handle == NULL || dev->CS_GPIO == NULL)
		return -RET_NULL_PTR;
	reg_addr &= 0x7F;
	SPI_CS_RESET(dev->CS_GPIO,dev->CS_Pin);
	SPI_Delay_us(1);
	HAL_SPI_Transmit(dev->handle,&reg_addr,1,PLATFORM_SPI_TIMEOUT_MS);
	HAL_SPI_Transmit(dev->handle,data,lenth,PLATFORM_SPI_TIMEOUT_MS);
	SPI_Delay_us(1);
	SPI_CS_SET(dev->CS_GPIO,dev->CS_Pin);
	return RET_OK;
}
//模式1是地址和数据中间关断spi外设，即地址先发送，在发送数据
int32_t platform_spi_read(struct platform_dev_spi_t *dev,uint8_t reg_addr,uint8_t *data, uint16_t lenth)
{
	uint8_t tx_buf[PLATFORM_SPI_READ_MAX_LEN] = {0xff};
	if(dev->handle == NULL || dev->CS_GPIO == NULL)
		return -RET_NULL_PTR;
	reg_addr|=0x80;
	SPI_CS_RESET(dev->CS_GPIO,dev->CS_Pin);
	SPI_Delay_us(1);
	HAL_SPI_Transmit(dev->handle,&reg_addr,1,PLATFORM_SPI_TIMEOUT_MS);
	HAL_SPI_TransmitReceive(dev->handle, tx_buf, data, lenth, PLATFORM_SPI_TIMEOUT_MS);
	SPI_Delay_us(1);
	SPI_CS_SET(dev->CS_GPIO,dev->CS_Pin);
	return RET_OK;
}

int32_t platform_write_cmd(struct platform_dev_spi_t *dev,const uint8_t *cmd,uint16_t cmd_len,const uint8_t *bufp,uint16_t lenth)
{
	SPI_CS_RESET(dev->CS_GPIO,dev->CS_Pin);
    HAL_SPI_Transmit(dev->handle, (uint8_t *)cmd, cmd_len, PLATFORM_SPI_TIMEOUT_MS);
    HAL_SPI_Transmit(dev->handle, (uint8_t *)bufp, lenth, PLATFORM_SPI_TIMEOUT_MS);
	SPI_CS_SET(dev->CS_GPIO,dev->CS_Pin);
    return RET_OK;
} 


int32_t platform_read_cmd(struct platform_dev_spi_t *dev,const uint8_t *cmd,uint16_t cmd_len,uint8_t *bufp,uint16_t lenth)
{
	uint8_t tx_buf[2112] = {0xff};
    SPI_CS_RESET(dev->CS_GPIO,dev->CS_Pin);
    HAL_SPI_Transmit(dev->handle, (uint8_t *)cmd, cmd_len, PLATFORM_SPI_TIMEOUT_MS);
    HAL_SPI_TransmitReceive(dev->handle,tx_buf,bufp,lenth,PLATFORM_SPI_TIMEOUT_MS);
    SPI_CS_SET(dev->CS_GPIO,dev->CS_Pin);
    return RET_OK;
}


//模式2是地址和数据中间不能关断spi外设，即地址和数据一起发送
int32_t platform_spi_read_mode2(struct platform_dev_spi_t *dev,uint8_t reg_addr,uint8_t *data, uint16_t lenth)
{
    int32_t ret;
    union
    {
        struct
        {
            uint8_t addr;
            uint8_t payload[PLATFORM_SPI_READ_MAX_LEN];
        } data;
        uint8_t buf[PLATFORM_SPI_READ_MAX_LEN + 1U];
    } tx = {0}, rx = {0};
    tx.data.addr = reg_addr | 0x80;
    SPI_CS_RESET(dev->CS_GPIO, dev->CS_Pin);
    SPI_Delay_us(1);
    HAL_SPI_TransmitReceive(dev->handle, tx.buf, rx.buf,(uint16_t)(lenth + 1U), PLATFORM_SPI_TIMEOUT_MS);
    SPI_Delay_us(1);
    SPI_CS_SET(dev->CS_GPIO, dev->CS_Pin);
    memcpy(data, rx.data.payload, lenth);
    return RET_OK;
}

