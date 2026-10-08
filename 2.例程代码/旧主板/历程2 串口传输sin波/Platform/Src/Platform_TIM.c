/**
  ******************************************************************************
  * @file           : Platform_TIM.c
  * @brief          : TIM平台层文件
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
#include "Platform_TIM.h"

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/

static int32_t platform_write(void *ctx, uint32_t key, const void *bufp, uint16_t len);
static int32_t platform_close(void *ctx);
stmdev_ctx_t platform_tim_dev =
{
    .handle = &htim7,
    .io = TIM_CHANNEL_3,
    .close = platform_close,
    .write = platform_write,

};

/* Function ------------------------------------------------------------------*/
/**
* @brief 通过TIM DMA输出RGB时序数据
* @param handle TIM句柄
* @param CHANNEL TIM通道
* @param bufp 时序数据指针
* @param len 时序数据长度
* @return 写入结果
*/
static int32_t platform_write(void *ctx, uint32_t key, const void *bufp, uint16_t len)
{
    const uint32_t *data = (const uint32_t *)bufp;
    if(ctx == NULL)
    {
    return -LY_ERR_INVALID;
    }
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)dev->handle;
    if(HAL_TIM_PWM_Start_DMA(htim, key, (uint32_t *)data, len) != HAL_OK)
    {
    return -LY_ERR_IO;
    }
    return LY_OK;
}

/**
* @brief 关闭RGB TIM DMA输出并复位计数器
* @param ctx 平台设备上下文
* @return 关闭结果
*/
static int32_t platform_close(void *ctx)
{
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    if(dev == 0)
          return -LY_ERR_INVALID;
    if(HAL_TIM_PWM_Stop_DMA((TIM_HandleTypeDef *)dev->handle, dev->io) != HAL_OK)
    {
      return -LY_ERR_IO;
    }
    __HAL_TIM_SetCounter((TIM_HandleTypeDef *)dev->handle,0);
    return LY_OK;
}
