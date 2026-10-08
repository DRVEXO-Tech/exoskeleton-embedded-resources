/**
  ******************************************************************************
  * @file           : Device_IO_Control.c
  * @brief          : IO控制设备层文件
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
#include "Device_IO_Control.h"


/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/
/**
* @brief 控制VM电源使能引脚
* @param data 0关闭，非0开启
* @return 控制结果
*/
int32_t Device_EN_VM_Write(uint8_t data)
{
    return platform_gpio_VM_dev.write_value(&platform_gpio_VM_dev, (uint32_t)data);
}
/**
* @brief 控制5V电源使能引脚
* @param data 0关闭，非0开启
* @return 控制结果
*/
int32_t Device_EN_5V_Write(uint8_t data)
{
    return platform_gpio_5V_dev.write_value(&platform_gpio_5V_dev, (uint32_t)data);
}



/**
* @brief 控制3V3电源使能引脚
* @param data 0关闭，非0开启
* @return 控制结果
*/
int32_t Device_EN_3V3_Write(uint8_t data)
{
    return platform_gpio_3V3_dev.write_value(&platform_gpio_3V3_dev, (uint32_t)data);
}

/**
* @brief 控制LED引脚电平
* @param data 0输出低电平，非0输出高电平
* @return 控制结果
*/
int32_t Device_LED_Write(uint8_t data)
{
    return platform_gpio_led_dev.write_value(&platform_gpio_led_dev, (uint32_t)data);
}


/**
* @brief 翻转LED引脚电平
* @param 无
* @return 控制结果
*/
int32_t Device_LED_Toggle(void)
{
    uint32_t data;

    if(platform_gpio_led_dev.read_value(&platform_gpio_led_dev, &data) != LY_OK)
    {
        return -LY_ERR;
    }

    return platform_gpio_led_dev.write_value(&platform_gpio_led_dev, (data == 0U) ? 1U : 0U);
}

/**
* @brief 初始化四个电机按键唤醒输入
* @param 无
* @return 初始化结果
*/
int32_t Device_WakeupKeys_Init(void)
{
    return Platform_WakeupKeys_Init();
}

/**
* @brief 读取四个电机按键唤醒输入的位掩码
* @param pressed_mask 输出按键位掩码，非零表示至少一个按键保持按下
* @return 读取结果
*/
int32_t Device_WakeupKeys_Read(uint8_t *pressed_mask)
{
    return Platform_WakeupKeys_Read(pressed_mask);
}
