/**
  ******************************************************************************
  * @file           : APP_LED.c
  * @brief          : LED应用层文件
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
#include "APP_LED.h"
#include "Sys.h"

/* Define ------------------------------------------------------------------*/
#define APP_LED_OFF_LEVEL    0U
#define APP_LED_ON_LEVEL     1U

/* Variable ------------------------------------------------------------------*/
static volatile APP_LED_Mode_en g_app_led_mode = APP_LED_MODE_BLINK;

/* Function ------------------------------------------------------------------*/
/**
* @brief 设置LED工作模式并立即执行常亮或常灭状态
* @param mode LED常灭、常亮或闪烁模式
* @return 0表示设置成功，负值表示参数或GPIO控制失败
*/
int32_t APP_LED_Mode_Set(APP_LED_Mode_en mode)
{
    int32_t ret = LY_OK;

    if((mode != APP_LED_MODE_OFF) &&
       (mode != APP_LED_MODE_ON) &&
       (mode != APP_LED_MODE_BLINK))
    {
        return -LY_ERR_INVALID;
    }

    g_app_led_mode = mode;
    if(mode == APP_LED_MODE_OFF)
    {
        ret = Device_LED_Write(APP_LED_OFF_LEVEL);
    }
    else if(mode == APP_LED_MODE_ON)
    {
        ret = Device_LED_Write(APP_LED_ON_LEVEL);
    }
    return ret;
}

/**
* @brief LED周期任务
* @param dT_ms 任务周期，单位ms
* @return 无
*/
void APP_LED_TASK(uint16_t dT_ms)
{
    if((dT_ms == 0U) || (g_app_led_mode != APP_LED_MODE_BLINK))
    {
        return;
    }
    if(Device_LED_Toggle() != LY_OK)
    {
        sys_err.LED_Task_err_cnt++;
    }
}
