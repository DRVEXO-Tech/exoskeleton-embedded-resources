/**
  ******************************************************************************
  * @file           : Platform_SPI.c
  * @brief          : SPI平台层文件
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
#include "Platform_SPI.h"
#include "Platform_GPIO.h"
/* Define ------------------------------------------------------------------*/
#define SPI_TIME_OUT                     1000U
#define BOOT_TIME                        10U

/* Function ------------------------------------------------------------------*/
static int32_t platform_write(void *handle, uint32_t key, const void *bufp,uint16_t len);
static int32_t platform_read(void *handle, uint32_t key, void *bufp,uint16_t len);
static int32_t platform_write_cmd(void *ctx,const uint8_t *cmd,uint16_t cmd_len,const uint8_t *bufp,uint16_t len);
static int32_t platform_read_cmd(void *ctx,const uint8_t *cmd,uint16_t cmd_len,uint8_t *bufp,uint16_t len);
static void platform_delay(uint32_t ms);

/* Variable ------------------------------------------------------------------*/
stmdev_ctx_t platform_spi_w25n01_dev={
    .handle = &hspi1,
    .priv_data = &platform_gpio_cs1_dev,
    .mdelay = platform_delay,
    .read_cmd = platform_read_cmd,
    .write_cmd = platform_write_cmd,
};
stmdev_ctx_t platform_spi_ism6dso1_dev={
    .handle = &hspi2,
    .priv_data = &platform_gpio_cs2_dev,
    .mdelay = platform_delay,
    .read = platform_read,
    .write = platform_write,
};
stmdev_ctx_t platform_spi_ism6dso2_dev={
    .handle = &hspi3,
    .priv_data = &platform_gpio_cs3_dev,
    .mdelay = platform_delay,
    .read = platform_read,
    .write = platform_write,
};

stmdev_ctx_t platform_spi_lps22df_dev={
    .handle = &hspi4,
    .priv_data = &platform_gpio_cs4_dev,
    .mdelay = platform_delay,
    .read = platform_read,
    .write = platform_write,
};


/* Function ------------------------------------------------------------------*/
/**
* @brief 向ISM6DSO写寄存器数据
* @param handle SPI句柄
* @param reg 寄存器地址
* @param bufp 写入数据指针
* @param len 写入数据长度
* @return 写入结果
*/
static int32_t platform_write(void *ctx, uint32_t key, const void *bufp,
                              uint16_t len)
{
    uint8_t reg = (uint8_t)key;
    const uint8_t *data = (const uint8_t *)bufp;
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    stmdev_ctx_t *priv =(stmdev_ctx_t *)dev ->priv_data;
    priv->write_value(priv,0);
    HAL_SPI_Transmit(dev->handle, &reg, 1, SPI_TIME_OUT);
    HAL_SPI_Transmit(dev->handle, (uint8_t*) data, len, SPI_TIME_OUT);
    priv->write_value(priv,1);
    return LY_OK;
}

/**
* @brief 从ISM6DSO读寄存器数据
* @param handle SPI句柄
* @param reg 寄存器地址
* @param bufp 读取数据缓冲区
* @param len 读取数据长度
* @return 读取结果
*/
static int32_t platform_read(void *ctx, uint32_t key, void *bufp,
                             uint16_t len)
{
    uint8_t reg = (uint8_t)key;
    uint8_t *data = (uint8_t *)bufp;
    reg |= 0x80;
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    stmdev_ctx_t *priv =(stmdev_ctx_t *)dev ->priv_data;
    priv->write_value(priv,0);
    HAL_SPI_Transmit(dev->handle, &reg, 1, SPI_TIME_OUT);
    HAL_SPI_Receive(dev->handle, data, len, SPI_TIME_OUT);
    priv->write_value(priv,1);
    return LY_OK;
}


/**
* @brief 向W25N01KV发送写命令
* @param handle SPI句柄
* @param cmd 命令数据指针
* @param cmd_len 命令长度
* @param bufp 数据指针
* @param len 数据长度
* @return 发送结果
*/
static int32_t platform_write_cmd(void *ctx,const uint8_t *cmd,uint16_t cmd_len,const uint8_t *bufp,uint16_t len)
{
    int32_t ret = 0;
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    stmdev_ctx_t *priv =(stmdev_ctx_t *)dev ->priv_data;
    priv->write_value(priv,0);
    if ((cmd_len != 0) &&(HAL_SPI_Transmit(dev->handle, (uint8_t *)cmd, cmd_len, SPI_TIME_OUT) != HAL_OK))
    {
    ret = -LY_ERR_IO;
    }
    if ((ret == 0) && (len != 0) &&(HAL_SPI_Transmit(dev->handle, (uint8_t *)bufp, len, SPI_TIME_OUT) != HAL_OK))
    {
    ret = -LY_ERR_IO;
    }
    priv->write_value(priv,1);
    return ret;
} 

/**
* @brief 向W25N01KV发送读命令并读取数据
* @param handle SPI句柄
* @param cmd 命令数据指针
* @param cmd_len 命令长度
* @param bufp 读取数据缓冲区
* @param len 读取数据长度
* @return 读取结果
*/
static int32_t platform_read_cmd(void *ctx,const uint8_t *cmd,uint16_t cmd_len,uint8_t *bufp,uint16_t len)
{
    int32_t ret = 0;

    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    stmdev_ctx_t *priv =(stmdev_ctx_t *)dev ->priv_data;
    priv->write_value(priv,0);

    if ((cmd_len != 0) &&(HAL_SPI_Transmit(dev->handle, (uint8_t *)cmd, cmd_len, SPI_TIME_OUT) != HAL_OK))
    {
    ret = -LY_ERR_IO;
    }

    if ((ret == 0) && (len != 0))
    {
    uint8_t dummy = 0xFF;
    for (uint16_t i = 0; i < len; i++)
    {
      if (HAL_SPI_TransmitReceive(dev->handle,&dummy,&bufp[i],1,SPI_TIME_OUT) != HAL_OK)
      {
        ret = -LY_ERR_IO;
        break;
      }
    }
    }

    priv->write_value(priv,1);

    return ret;
}




/**
* @brief 平台毫秒延时
* @param ms 延时时间
* @return 无
*/
static void platform_delay(uint32_t ms)
{
  HAL_Delay(ms);
}

