/**
  ******************************************************************************
  * @file           : APP_VOICE.c
  * @brief          : 语音APP层文件
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
#include "APP_VOICE.h"
#include "Sys.h"
/* Define ------------------------------------------------------------------*/
#define APP_VOICE_QUEUE_SIZE         15U
#define APP_VOICE_READY_DELAY_MS     2000U
#define APP_VOICE_BATTERY_LEVEL_MAX  100U
#define APP_VOICE_ID_BATTERY_0       106U

/* Variable ------------------------------------------------------------------*/
static uint8_t g_voice_queue[APP_VOICE_QUEUE_SIZE]={0};
static uint8_t g_voice_queue_head=0;
static uint8_t g_voice_queue_tail=0;

/* Function ------------------------------------------------------------------*/
/**
* @brief 向语音队列压入一条语音
* @param voice_id 语音ID
* @return 0表示入队成功，负值表示语音ID无效或队列已满
*/
int32_t app_voice_enqueue(uint8_t voice_id)
{
    uint8_t next_tail = (uint8_t)((g_voice_queue_tail + 1U) % APP_VOICE_QUEUE_SIZE);

    if (voice_id == 0U)
    {
        return -LY_ERR_INVALID;
    }
    if (next_tail == g_voice_queue_head)
    {
        return -LY_ERR_BUSY;
    }

    g_voice_queue[g_voice_queue_tail] = voice_id;
    g_voice_queue_tail = next_tail;
    return LY_OK;
}

/**
* @brief 立即发送当前电池电量语音，供进入用户主函数前播报
* @param battery_level 电池电量百分比，范围为0至100
* @return 0表示发送成功，负值表示电量无效或语音发送失败
*/
int32_t APP_Voice_PlayBatteryLevel(uint8_t battery_level)
{
    uint8_t voice_id;

    if (battery_level > APP_VOICE_BATTERY_LEVEL_MAX)
    {
        return -LY_ERR_INVALID;
    }

    voice_id = (battery_level == 0U) ? APP_VOICE_ID_BATTERY_0 : battery_level;
    return voicex_play_voice(0U, voice_id);
}

/**
* @brief 处理语音播放队列
* @param 无
* @return 无
*/
static void app_voice_process_queue(void)
{
    /* Busy 拉高时说明芯片仍在播放，先不发下一条。 */
    if (voicex_is_busy() == BUSY)
    {
        return;
    }
    if (g_voice_queue_head != g_voice_queue_tail)
    {
        if (voicex_play_voice(0U, g_voice_queue[g_voice_queue_head]) != LY_OK)
        {
            sys_err.Voice_Play_err_cnt++;
            return;
        }
        g_voice_queue_head = (uint8_t)((g_voice_queue_head + 1) % APP_VOICE_QUEUE_SIZE);
    }
}

/**
* @brief 语音APP层任务
* @param dT_ms 任务周期
* @return 无
*/
void APP_Voice_Task(uint16_t dT_ms)
{
    static uint8_t ready_delay_started = 0U;
    static uint32_t ready_tick = 0U;

    (void)dT_ms;

    if (sys_ready.all_ready != 1U)
    {
        ready_delay_started = 0U;
        return;
    }

    if (ready_delay_started == 0U)
    {
        ready_tick = HAL_GetTick();
        ready_delay_started = 1U;
        return;
    }

    if ((uint32_t)(HAL_GetTick() - ready_tick) < APP_VOICE_READY_DELAY_MS)
    {
        return;
    }

    app_voice_process_queue();
}

