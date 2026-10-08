/**
  ******************************************************************************
  * @file           : APP_LowPower.c
  * @brief          : 主控低功耗应用层文件
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
#include "APP_LowPower.h"
#include "Device_FDCAN.h"
#include "Device_IO_Control.h"
#include "Sys.h"
#include "User_main.h"
#include "config.h"
#if (LOWPOWER_FLAG_DEBUG == 1U)
#include "stm32h5xx_ll_system.h"
#endif

/* Define ------------------------------------------------------------------*/
#define APP_LOWPOWER_WAKE_CONFIRM_MS      1000U
#define APP_LOWPOWER_WAKE_SAMPLE_MS       10U
#define APP_LOWPOWER_SYSTEM_POWER_OFF     0U

#if ((USER_POWER_ON_STANDBY_ENABLE != 0U) && (USER_POWER_ON_STANDBY_ENABLE != 1U))
#error "USER_POWER_ON_STANDBY_ENABLE must be 0U or 1U"
#endif

/* Variable ------------------------------------------------------------------*/
static uint8_t app_lowpower_sleep_pending = 0U;

/* Function ------------------------------------------------------------------*/
/**
* @brief 配置按键唤醒源并进入Standby模式
* @param 无
* @return 正常情况下不返回，负值表示进入Standby失败
*/
static int32_t app_lowpower_enter_standby(void)
{
    PWREx_WakeupPinTypeDef wakeup_pin = {0};

    if (Device_WakeupKeys_Init() != LY_OK)
    {
        sys_err.LowPower_IO_err_cnt++;
        return -LY_ERR;
    }

#if (LOWPOWER_FLAG_DEBUG == 1U)
    LL_DBGMCU_EnableDBGStandbyMode();
#endif

    HAL_PWREx_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    HAL_PWREx_DisableWakeUpPin(PWR_WAKEUP_PIN2);
    HAL_PWREx_DisableWakeUpPin(PWR_WAKEUP_PIN4);
    HAL_PWREx_DisableWakeUpPin(PWR_WAKEUP_PIN6);
    __HAL_PWR_CLEAR_FLAG(PWR_WAKEUP_ALL_FLAG);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SBF);

    wakeup_pin.PinPolarity = PWR_PIN_POLARITY_HIGH;
    wakeup_pin.PinPull = PWR_PIN_PULL_DOWN;
    wakeup_pin.WakeUpPin = PWR_WAKEUP_PIN1;
    HAL_PWREx_EnableWakeUpPin(&wakeup_pin);
    wakeup_pin.WakeUpPin = PWR_WAKEUP_PIN2;
    HAL_PWREx_EnableWakeUpPin(&wakeup_pin);
    wakeup_pin.WakeUpPin = PWR_WAKEUP_PIN4;
    HAL_PWREx_EnableWakeUpPin(&wakeup_pin);
    wakeup_pin.WakeUpPin = PWR_WAKEUP_PIN6;
    HAL_PWREx_EnableWakeUpPin(&wakeup_pin);

    if (Device_EN_VM_Write(0U) != LY_OK)
    {
        sys_err.LowPower_IO_err_cnt++;
    }
    if (Device_EN_5V_Write(0U) != LY_OK)
    {
        sys_err.LowPower_IO_err_cnt++;
    }
    if (Device_EN_3V3_Write(0U) != LY_OK)
    {
        sys_err.LowPower_IO_err_cnt++;
    }

    __HAL_PWR_CLEAR_FLAG(PWR_WAKEUP_ALL_FLAG);
    __disable_irq();
    HAL_PWR_EnterSTANDBYMode();
    __enable_irq();

    sys_err.LowPower_Standby_err_cnt++;
    return -LY_ERR;
}

/**
* @brief 初始化电机关机检测与按键唤醒输入
* @param 无
* @return 初始化结果
*/
int32_t APP_LowPower_Init(void)
{
    app_lowpower_sleep_pending = 0U;
    return Device_WakeupKeys_Init();
}

/**
* @brief 在Standby唤醒复位后确认任意按键持续按住一秒
* @param 无
* @return LY_OK表示普通启动或唤醒确认成功，负值表示确认过程失败
*/
int32_t APP_LowPower_EarlyWakeConfirm(void)
{
    uint32_t confirm_start_tick;
    uint8_t pressed_mask;
    uint8_t confirm_mask;

    if (__HAL_PWR_GET_FLAG(PWR_FLAG_SBF) == 0U)
    {
#if (USER_POWER_ON_STANDBY_ENABLE == 1U)
        return app_lowpower_enter_standby();
#else
        return LY_OK;
#endif
    }

    if (Device_WakeupKeys_Init() != LY_OK)
    {
        return -LY_ERR;
    }
    if (Device_WakeupKeys_Read(&pressed_mask) != LY_OK)
    {
        return -LY_ERR;
    }
    if (pressed_mask == 0U)
    {
        return app_lowpower_enter_standby();
    }

    confirm_mask = pressed_mask;
    confirm_start_tick = HAL_GetTick();
    while ((uint32_t)(HAL_GetTick() - confirm_start_tick) < APP_LOWPOWER_WAKE_CONFIRM_MS)
    {
        if (Device_WakeupKeys_Read(&pressed_mask) != LY_OK)
        {
            return -LY_ERR;
        }
        confirm_mask &= pressed_mask;
        if (confirm_mask == 0U)
        {
            return app_lowpower_enter_standby();
        }
        HAL_Delay(APP_LOWPOWER_WAKE_SAMPLE_MS);
    }

    HAL_PWREx_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    HAL_PWREx_DisableWakeUpPin(PWR_WAKEUP_PIN2);
    HAL_PWREx_DisableWakeUpPin(PWR_WAKEUP_PIN4);
    HAL_PWREx_DisableWakeUpPin(PWR_WAKEUP_PIN6);
    __HAL_PWR_CLEAR_FLAG(PWR_WAKEUP_ALL_FLAG);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SBF);

    return LY_OK;
}

/**
* @brief 检测电机关机状态并在原按键释放后进入Standby
* @param dT_ms 任务周期
* @return 无
*/
void APP_LowPower_Task(uint16_t dT_ms)
{
    uint8_t pressed_mask;

    (void)dT_ms;

    if (app_lowpower_sleep_pending == 0U)
    {
        if ((sys_ready.Left_Motor_Ready != 0U) &&
            (sys_ready.Right_Motor_Ready != 0U) &&
            ((fdcan_get_syspower_flag() == APP_LOWPOWER_SYSTEM_POWER_OFF) ||
             (fdcan_get_syspower_flag_r() == APP_LOWPOWER_SYSTEM_POWER_OFF)))
        {
            app_lowpower_sleep_pending = 1U;
        }
        return;
    }

    if (Device_WakeupKeys_Read(&pressed_mask) != LY_OK)
    {
        sys_err.LowPower_IO_err_cnt++;
        return;
    }

    if (pressed_mask == 0U)
    {
        (void)app_lowpower_enter_standby();
    }
}
