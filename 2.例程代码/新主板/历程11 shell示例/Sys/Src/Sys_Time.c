/**
  ******************************************************************************
  * @file           : Sys_Time.c
  * @brief          : 系统计时文件
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
#include "Sys_Time.h"

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/
static volatile uint32_t cnt_time = 0U;
/* Function ------------------------------------------------------------------*/
/* 由TIM7中断更新毫秒计数。 */
void Sys_IncTick_ms(void)
{
  cnt_time += 1U;
}
/* 读取供主循环调度使用的毫秒计数。 */
uint32_t GetTick_ms(void)
{
    return cnt_time;

}

/**
 * @brief  初始化DWT周期计数器。
 * @return 错误码。
 */
int32_t Sys_Time_Init_us(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    if ((DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk) != 0U)
    {
        return -RET_NOT_SUPPORT;
    }
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __DSB();
    __ISB();
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0U)
    {
        return -RET_NOT_READY;
    }
    return RET_OK;
}

/**
 * @brief  保存计时起点。
 * @return 原始CPU周期数。
 */
uint32_t Sys_Time_Start_us(void)
{
    return DWT->CYCCNT;
}

/**
 * @brief  结束一次测量并计算微秒耗时。
 * @param  start_cycle Sys_Time_Start_us返回的周期数。
 * @return 耗时，单位us。
 */
uint32_t Sys_Time_End_us(uint32_t start_cycle)
{
    uint32_t elapsed_cycle;

    /* 先计算无符号周期差，支持跨越一次计数回绕。 */
    elapsed_cycle = (uint32_t)(DWT->CYCCNT - start_cycle);

    /* 使用64位中间结果，避免乘法溢出。 */
    return (uint32_t)(((uint64_t)elapsed_cycle * 1000000ULL)/ SystemCoreClock);
}
uint32_t Sys_Delay_us(uint16_t us)
{
	 uint32_t start_cycle;
    uint32_t delay_cycle;

    if (us == 0U)
        return RET_OK;

    if (((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0U) ||
        ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0U))
    {
        return -RET_NOT_INIT;
    }

    /* 使用64位乘法并向上取整，避免溢出和周期换算不足。 */
    delay_cycle = (uint32_t)(
        ((uint64_t)SystemCoreClock * us + 999999ULL) / 1000000ULL);

    start_cycle = DWT->CYCCNT;

    /* 无符号差值可处理计数器回绕，不清零共享计数器。 */
    while ((uint32_t)(DWT->CYCCNT - start_cycle) < delay_cycle)
    {
        __NOP();
    }

    return RET_OK;
}