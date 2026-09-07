/**
  ******************************************************************************
  * @file    Device_FDCAN.h
  * @brief   Device_FDCAN驱动层头文件。
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 DRVEXO.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_FDCAN_H__
#define __DEVICE_FDCAN_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Platform_FDCAN.h"

/* Private defines -----------------------------------------------------------*/
#define FDCANX_LEFT_MOTOR_ID             0x000U
#define FDCANX_RIGHT_MOTOR_ID            0x001U
#define FDCANX_LEFT_MOTOR_REPLY_ID       0x002U
#define FDCANX_RIGHT_MOTOR_REPLY_ID      0x003U
#define FDCANX_CORE_CONTROL_LEFT_ID      0x300U
#define FDCANX_CORE_CONTROL_RIGHT_ID     0x301U
#define FDCANX_CORE_SETTING_LEFT_ID      0x310U
#define FDCANX_CORE_SETTING_RIGHT_ID     0x311U
#define FDCANX_CORE_ID                   0x302U
#define FDCANX_BATTERY_ID                0x303U
#define FDCANX_CORE_CONTROL_MAX_LEN      24U

/* Exported types ------------------------------------------------------------*/
struct Bat_Data_st
{
    uint32_t pd_power;             //IP2366充放电功率  单位mW
    uint32_t pd_voltage;           //IP2366usb电压     单位mV
    uint32_t bat_voltage;          //电池侧电压        单位mV
    int32_t  bat_current;          //电池侧电流        单位mA
    uint8_t  bat_capacity;         //电池剩余电量      单位%
    uint16_t bat_cell_voltage[6];  //各个电池电压      单位mV
    uint16_t bat_temperature[4];   //温度              单位摄氏度
    uint8_t  bat_charging_state;   //读取电池充放电状态 0：放电 1：充电
    uint8_t  bat_health;           //电池健康度        单位%
    uint16_t bat_estimated_remaining_charging_time;    //预计剩余充电时间 单位min
    uint16_t bat_estimated_remaining_discharging_time; //预计剩余放电时间 单位min
    uint16_t bat_cycle;            //充放电循环次数    单位次
};

#pragma pack(push, 1)
typedef union
{
    uint8_t buf[48];
    struct Bat_Data_st data;
} Battery_SendAlways_u;

typedef union
{
    struct
    {
        uint16_t fault_code;
        uint8_t button;
        uint16_t sys_torque;
        float input_angle;
        float output_angle;
        float torque;
        float current;
        float input_velocity;
        float input_acceleration;
        float output_velocity;
        float output_acceleration;
        float temperature;
    } data;
    uint8_t buf[48];
} Motor_SendAlways_u;

typedef union
{
    struct
    {
        uint32_t time;
    } data;
    uint8_t buf[4];
} Core_SendAlways_u;

typedef union
{
    struct
    {
        uint32_t data1;
        uint32_t data2;
        uint32_t data3;
        uint32_t data4;
        uint32_t data5;
        uint32_t data6;
    } data;
    uint8_t buf[24];
} Core_SendControlData_u;

#pragma pack(pop)

/* Exported functions prototypes ---------------------------------------------*/
int32_t fdcan_init(void);
int32_t fdcan_write(uint16_t id, const uint8_t *data, uint8_t len);
int32_t fdcan_irq_handler(void);
int32_t fdcan_recover_busoff(void);

/* 普通控制命令ACK缓存 */
void fdcan_write_receive_flag(uint8_t dir,uint8_t value);
int32_t fdcan_get_receive_flag(uint8_t dir);
int32_t fdcan_get_motor_receive_one_data(uint8_t dir);

int32_t fdcan_get_battery_data(Battery_SendAlways_u *battery);
int32_t fdcan_get_left_motor_data(Motor_SendAlways_u *motor);
int32_t fdcan_get_right_motor_data(Motor_SendAlways_u *motor);

uint16_t fdcan_get_total_vol(void);

uint8_t fdcan_get_syspower_flag(void);
uint8_t fdcan_get_sysmode(void);
uint16_t fdcan_get_left_motor_sys_torque(void);
uint8_t fdcan_get_syspower_flag_r(void);
uint8_t fdcan_get_sysmode_r(void);
uint16_t fdcan_get_right_motor_sys_torque(void);
uint32_t fdcan_get_left_motor_dt_ms(void);
uint32_t fdcan_get_right_motor_dt_ms(void);
#ifdef __cplusplus
}
#endif
#endif   /* __DEVICE_FDCAN_H__ */
