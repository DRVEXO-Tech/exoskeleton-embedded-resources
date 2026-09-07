/**
  ******************************************************************************
  * @file           : Device_FDCAN.c
  * @brief          : FDCAN驱动层文件
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
#include "Device_FDCAN.h"
#include <string.h>

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/
/**
* @brief 初始化FDCAN驱动层
* @param 无
* @return 初始化结果
*/
int32_t fdcan_init(void)
{
    if (platform_fdcan_dev.init == 0)
    {
        return -LY_ERR;
    }

    return platform_fdcan_dev.init(&platform_fdcan_dev);
}

/**
* @brief 发送FDCAN数据
* @param id 标准帧ID
* @param data 数据指针
* @param len 数据长度
* @return 发送结果
*/
int32_t fdcan_write(uint16_t id, const uint8_t *data, uint8_t len)
{
    if (platform_fdcan_dev.write == 0)
    {
        return -LY_ERR;
    }

    return platform_fdcan_dev.write(&platform_fdcan_dev, id, data, len);
}

/**
* @brief 处理FDCAN中断回调
* @param 无
* @return 处理结果
*/
int32_t fdcan_irq_handler(void)
{
    if (platform_fdcan_dev.callback == 0)
    {
        return -LY_ERR;
    }

    return platform_fdcan_dev.callback(&platform_fdcan_dev);
}

int32_t fdcan_recover_busoff(void)
{
    return Platform_FDCAN_RecoverBusOff();
}

/**
* @brief 写入电机接收标志
* @param dir 电机方向
* @param value 标志值
* @return 无
*/
void fdcan_write_receive_flag(uint8_t dir,uint8_t value)
{
    if(dir == 0)
        platform_extern_data.Leftmotor_receive_flag = value;
    if(dir == 1)
        platform_extern_data.Rightmotor_receive_flag =value;
}


/**
* @brief 获取电机接收标志
* @param dir 电机方向
* @return 接收标志
*/
int32_t fdcan_get_receive_flag(uint8_t dir)
{
    if(dir == 0)
        return platform_extern_data.Leftmotor_receive_flag;
    else if(dir == 1)
        return platform_extern_data.Rightmotor_receive_flag;
    return -LY_ERR_INVALID;
}

/**
* @brief 获取电机普通单字节回复数据
* @param dir 电机方向
* @return 普通单字节回复数据，方向错误返回负值
*/
int32_t fdcan_get_motor_receive_one_data(uint8_t dir)
{
    if(dir == 0)
        return platform_extern_data.Leftmotor_receive_one_data;
    else if(dir == 1)
        return platform_extern_data.Rightmotor_receive_one_data;
    return -LY_ERR_INVALID;
}

/**
* @brief 获取电池完整周期数据
* @param battery 电池数据输出指针
* @return 获取结果
*/
int32_t fdcan_get_battery_data(Battery_SendAlways_u *battery)
{
    if (battery == 0)
    {
        return -LY_ERR_INVALID;
    }
    if (sys_ready.Bat_Ready == 0U)
    {
        return LY_NO_DATA;
    }

    memcpy(battery->buf, platform_extern_data.Batter_buf, PLATFORM_FDCAN_BAT_BUF_LEN);
    return LY_OK;
}

/**
* @brief 获取左电机完整周期数据
* @param motor 左电机数据输出指针
* @return 获取结果
*/
int32_t fdcan_get_left_motor_data(Motor_SendAlways_u *motor)
{
    if (motor == 0)
    {
        return -LY_ERR_INVALID;
    }

    memcpy(motor->buf, platform_extern_data.Leftmotor_buf, PLATFORM_FDCAN_MOTOR_BUF_LEN);
    return LY_OK;
}

/**
* @brief 获取右电机完整周期数据
* @param motor 右电机数据输出指针
* @return 获取结果
*/
int32_t fdcan_get_right_motor_data(Motor_SendAlways_u *motor)
{
    if (motor == 0)
    {
        return -LY_ERR_INVALID;
    }

    memcpy(motor->buf, platform_extern_data.Rightmotor_buf, PLATFORM_FDCAN_MOTOR_BUF_LEN);
    return LY_OK;
}

/**
 * @brief 获取电池总电压
 * @param 无
 * @return 总电压
*/
uint16_t fdcan_get_total_vol(void)
{
    Battery_SendAlways_u battery;

    memcpy(battery.buf, platform_extern_data.Batter_buf, PLATFORM_FDCAN_BAT_BUF_LEN);
    return (uint16_t)battery.data.bat_voltage;
}

/**
* @brief 获取左电机系统电源状态
* @param 无
* @return 系统电源状态原始值
*/
uint8_t fdcan_get_syspower_flag(void)
{
    Motor_SendAlways_u motor;

    memcpy(motor.buf, platform_extern_data.Leftmotor_buf, sizeof(platform_extern_data.Leftmotor_buf));
    return (uint8_t)(motor.data.button >> 4U);
}

/**
* @brief 获取左电机当前模式
* @param 无
* @return 电机模式原始值
*/
uint8_t fdcan_get_sysmode(void)
{
    Motor_SendAlways_u motor;

    memcpy(motor.buf, platform_extern_data.Leftmotor_buf, sizeof(platform_extern_data.Leftmotor_buf));
    return (uint8_t)(motor.data.button & 0x0FU);
}

/**
* @brief 获取左电机当前力矩挡位
* @param 无
* @return 力矩挡位原始值
*/
uint16_t fdcan_get_left_motor_sys_torque(void)
{
    Motor_SendAlways_u motor;

    memcpy(motor.buf, platform_extern_data.Leftmotor_buf, sizeof(platform_extern_data.Leftmotor_buf));
    return motor.data.sys_torque;
}

/**
* @brief 获取右电机系统电源状态
* @param 无
* @return 系统电源状态原始值
*/
uint8_t fdcan_get_syspower_flag_r(void)
{
    Motor_SendAlways_u motor;

    memcpy(motor.buf, platform_extern_data.Rightmotor_buf, sizeof(platform_extern_data.Rightmotor_buf));
    return (uint8_t)(motor.data.button >> 4U);
}

/**
* @brief 获取右电机当前模式
* @param 无
* @return 电机模式原始值
*/
uint8_t fdcan_get_sysmode_r(void)
{
    Motor_SendAlways_u motor;

    memcpy(motor.buf, platform_extern_data.Rightmotor_buf, sizeof(platform_extern_data.Rightmotor_buf));
    return (uint8_t)(motor.data.button & 0x0FU);
}

/**
* @brief 获取右电机当前力矩挡位
* @param 无
* @return 力矩挡位原始值
*/
uint16_t fdcan_get_right_motor_sys_torque(void)
{
    Motor_SendAlways_u motor;

    memcpy(motor.buf, platform_extern_data.Rightmotor_buf, sizeof(platform_extern_data.Rightmotor_buf));
    return motor.data.sys_torque;
}

/**
* @brief 获取左电机周期数据接收间隔
* @param 无
* @return 接收间隔，单位ms
*/
uint32_t fdcan_get_left_motor_dt_ms(void)
{
    return platform_extern_data.Leftmotor_dt_ms;
}

/**
* @brief 获取右电机周期数据接收间隔
* @param 无
* @return 接收间隔，单位ms
*/
uint32_t fdcan_get_right_motor_dt_ms(void)
{
    return platform_extern_data.Rightmotor_dt_ms;
}
