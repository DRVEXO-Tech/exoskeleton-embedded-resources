/**
  ******************************************************************************
  * @file           : Platform_UART.c
  * @brief          : UART平台层文件
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 DRVEXO.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "Platform_UART.h"
#include "Platform_GPIO.h"

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/
static int32_t platform_write(void *ctx, uint32_t key, const void *bufp, uint16_t len);
static int32_t platform_write_dma(void *ctx, uint32_t key, const void *bufp, uint16_t len);

stmdev_ctx_t platform_uart_voice_dev =
{
    .handle = &huart6,
    .write = platform_write,
    .priv_data = &platform_gpio_voice_busy_dev,
};

stmdev_ctx_t platform_uart_user_dev =
{
    .handle = &huart1,
    .write = platform_write_dma,
};

/**
 * @brief 通过DMA发送用户串口数据
 * @param ctx 串口设备上下文
 * @param key 保留参数
 * @param bufp 发送数据指针
 * @param len 发送数据长度
 * @return 发送结果
 */
static int32_t platform_write_dma(void *ctx, uint32_t key, const void *bufp, uint16_t len)
{
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    const uint8_t *data = (const uint8_t *)bufp;

    (void)key;
    if ((dev == NULL) || (bufp == NULL) || (len == 0U) || (len > PLATFORM_UART_TX_BUF_SIZE))
    {
        return -LY_ERR_INVALID;
    }
    if (HAL_UART_Transmit_DMA(dev->handle, data, len) != HAL_OK)
    {
        return -LY_ERR_IO;
    }
    return LY_OK;
}

/**
 * @brief 阻塞发送语音模块数据
 * @param ctx 串口设备上下文
 * @param key 保留参数
 * @param bufp 发送数据指针
 * @param len 发送数据长度
 * @return 发送结果
 */
static int32_t platform_write(void *ctx, uint32_t key, const void *bufp, uint16_t len)
{
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    const uint8_t *data = (const uint8_t *)bufp;

    (void)key;
    if ((dev == NULL) || (bufp == NULL) || (len == 0U))
    {
        return -LY_ERR_INVALID;
    }
    if (HAL_UART_Transmit(dev->handle, data, len, HAL_MAX_DELAY) != HAL_OK)
    {
        return -LY_ERR_IO;
    }
    return LY_OK;
}

/**
 * @brief 读取语音模块忙状态
 * @param ctx 语音串口设备上下文
 * @return 忙状态
 */
uint8_t platform_is_busy(void *ctx)
{
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    stmdev_ctx_t *priv = (stmdev_ctx_t *)dev->priv_data;
    uint32_t data;

    priv->read_value(priv, &data);
    return (uint8_t)data;
}
