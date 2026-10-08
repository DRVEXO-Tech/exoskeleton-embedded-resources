/**
  ******************************************************************************
  * @file           : Platform_GPIO.c
  * @brief          : GPIO平台层c文件
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

#include "Platform_GPIO.h"

/* Define ------------------------------------------------------------------*/


/* Variable ------------------------------------------------------------------*/
static int32_t platform_write(void *handle,const uint32_t data);
static int32_t platform_read(void *handle,uint32_t *data);
//外置flash
stmdev_ctx_t platform_gpio_flash_wp_dev =
{
    .handle = FLASH_WP_GPIO_Port,
    .io = FLASH_WP_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
stmdev_ctx_t platform_gpio_flash_hold_dev =
{
    .handle = FLASH_HOLD_GPIO_Port,
    .io = FLASH_HOLD_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
//spi的片选
stmdev_ctx_t platform_gpio_cs1_dev =
{
    .handle = SPI1_CS_GPIO_Port,
    .io = SPI1_CS_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
stmdev_ctx_t platform_gpio_cs2_dev =
{
    .handle = SPI2_CS_GPIO_Port,
    .io = SPI2_CS_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
stmdev_ctx_t platform_gpio_cs3_dev =
{
    .handle = SPI3_CS_GPIO_Port,
    .io = SPI3_CS_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
stmdev_ctx_t platform_gpio_cs4_dev =
{
    .handle = SPI4_CS_GPIO_Port,
    .io = SPI4_CS_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};

//电源控制
stmdev_ctx_t platform_gpio_VM_dev =
{
    .handle = EN_VM_GPIO_Port,
    .io = EN_VM_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
stmdev_ctx_t platform_gpio_5V_dev =
{
    .handle = EN_5V_GPIO_Port,
    .io = EN_5V_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
stmdev_ctx_t platform_gpio_3V3_dev =
{
    .handle = EN_3V3_GPIO_Port,
    .io = EN_3V3_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
//FDCAN
stmdev_ctx_t platform_gpio_fdcan_stb_dev =
{
    .handle = FDCAN_STB_GPIO_Port,
    .io = FDCAN_STB_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
stmdev_ctx_t platform_gpio_fdcan2_stb_dev =
{
    .handle = FDCAN2_STB_GPIO_Port,
    .io = FDCAN2_STB_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
//语音空闲引脚
stmdev_ctx_t platform_gpio_voice_busy_dev =
{
    .handle = VOICE_BUSY_GPIO_Port,
    .io = VOICE_BUSY_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
//LED
stmdev_ctx_t platform_gpio_led_dev =
{
    .handle = LED_GPIO_Port,
    .io = LED_Pin,
    .write_value = platform_write,
    .read_value = platform_read,
};
/* Function ------------------------------------------------------------------*/

/**
* @brief 写GPIO单个引脚电平
* @param ctx GPIO平台设备上下文
* @param data 0为低电平，非0为高电平
* @return 写入结果
*/
static int32_t platform_write(void *ctx,const uint32_t data)
{
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev->handle;
    GPIO_PinState pin_state;

    if (gpio_port == 0)
    {
        return -LY_ERR_INVALID;
    }

    pin_state = (data == 0U) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(gpio_port, (uint16_t)dev->io, pin_state);

    return LY_OK;
}

/**
* @brief 读取GPIO单个引脚电平
* @param ctx GPIO平台设备上下文
* @param data 读取到的电平
* @return 读取结果
*/
static int32_t platform_read(void *ctx,uint32_t *data)
{
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev->handle;

    if ((gpio_port == 0) || (data == 0))
    {
        return -LY_ERR_INVALID;
    }

    *data = (uint32_t)HAL_GPIO_ReadPin(gpio_port, (uint16_t)dev->io);

    return LY_OK;
}

/**
* @brief 将四个电机按键唤醒引脚配置为下拉输入
* @param 无
* @return 配置结果
*/
int32_t Platform_WakeupKeys_Init(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_PULLDOWN;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

    gpio_init.Pin = LEFT_WKUP1_Pin;
    HAL_GPIO_Init(LEFT_WKUP1_GPIO_Port, &gpio_init);
    gpio_init.Pin = LEFT_WKUP2_Pin;
    HAL_GPIO_Init(LEFT_WKUP2_GPIO_Port, &gpio_init);
    gpio_init.Pin = RIGHT_WKUP1_Pin;
    HAL_GPIO_Init(RIGHT_WKUP1_GPIO_Port, &gpio_init);
    gpio_init.Pin = RIGHT_WKUP2_Pin;
    HAL_GPIO_Init(RIGHT_WKUP2_GPIO_Port, &gpio_init);

    return LY_OK;
}

/**
* @brief 读取四个电机按键唤醒引脚的位掩码
* @param pressed_mask 输出按键位掩码，位0至位3分别对应左1、左2、右1、右2
* @return 读取结果
*/
int32_t Platform_WakeupKeys_Read(uint8_t *pressed_mask)
{
    uint8_t key_mask = 0U;

    if (pressed_mask == 0)
    {
        return -LY_ERR_INVALID;
    }

    if (HAL_GPIO_ReadPin(LEFT_WKUP1_GPIO_Port, LEFT_WKUP1_Pin) == GPIO_PIN_SET)
    {
        key_mask |= (1U << 0U);
    }
    if (HAL_GPIO_ReadPin(LEFT_WKUP2_GPIO_Port, LEFT_WKUP2_Pin) == GPIO_PIN_SET)
    {
        key_mask |= (1U << 1U);
    }
    if (HAL_GPIO_ReadPin(RIGHT_WKUP1_GPIO_Port, RIGHT_WKUP1_Pin) == GPIO_PIN_SET)
    {
        key_mask |= (1U << 2U);
    }
    if (HAL_GPIO_ReadPin(RIGHT_WKUP2_GPIO_Port, RIGHT_WKUP2_Pin) == GPIO_PIN_SET)
    {
        key_mask |= (1U << 3U);
    }

    *pressed_mask = key_mask;

    return LY_OK;
}
