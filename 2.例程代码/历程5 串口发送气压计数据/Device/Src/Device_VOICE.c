/**
  ******************************************************************************
  * @file           : Device_VOICE.c
  * @brief          : 语音驱动层文件
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
#include "Device_VOICE.h"

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/
static uint8_t g_voice_tx_buf[7];

/* Function ------------------------------------------------------------------*/
/**
* @brief 发送语音模块数据
* @param data 数据指针
* @param length 数据长度
* @return 发送结果
*/
int32_t voicex_write(const uint8_t *data, uint16_t length)
{
    if ((platform_uart_voice_dev.write == 0) || (data == 0))
    {
        return -LY_ERR_INVALID;
    }

    return platform_uart_voice_dev.write(&platform_uart_voice_dev, 0U, data, length);
}

/**
* @brief 获取语音模块忙状态
* @param 无
* @return 忙状态
*/
VOICE_Status_Enum voicex_is_busy(void)
{
    if (platform_is_busy(&platform_uart_voice_dev) != 0U)
    {
        return BUSY;
    }

    return IDLE;
}
/**
 * @brief 播放指定文件中的指定语音。
 * @param file_id 文件编号。
 * @param voice_id 语音编号。
 */
int32_t voicex_play_voice(uint8_t file_id, uint8_t voice_id)
{
    if (voice_id == 0U)
    {
        return -LY_ERR_INVALID;
    }

    g_voice_tx_buf[0] = 0x7E;
    g_voice_tx_buf[1] = 0x05;
    g_voice_tx_buf[2] = 0x41;
    g_voice_tx_buf[3] = file_id;
    g_voice_tx_buf[4] = voice_id;
    g_voice_tx_buf[5] = (uint8_t)(g_voice_tx_buf[1] ^
                                  g_voice_tx_buf[2] ^
                                  g_voice_tx_buf[3] ^
                                  g_voice_tx_buf[4]);
    g_voice_tx_buf[6] = 0xEF;

    return voicex_write(g_voice_tx_buf,7);
}
