/**
  ******************************************************************************
  * @file    User_Data.h
  * @brief   用户数据头文件。
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
#ifndef __USER_DATA_H__
#define __USER_DATA_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private defines -----------------------------------------------------------*/
#define USER_MOTOR_COMMAND_TIMEOUT_MS    100U
#define USER_UART_JUSTFLOAT_MAX_CHANNELS 255U

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    USER_MOTOR_LEFT = 0,
    USER_MOTOR_RIGHT = 1
} User_Motor_direction_en;

typedef enum
{
    USER_MOTOR_CONTROL_MODE_TORQUE = 0,
    USER_MOTOR_CONTROL_MODE_VELOCITY = 1,
    USER_MOTOR_CONTROL_MODE_POSITION = 2
} User_Motor_control_mode_en;

typedef enum
{
    USER_MOTOR_SYSTEM_MODE_UNKNOWN = 0,
    USER_MOTOR_SYSTEM_MODE_REST = 1,
    USER_MOTOR_SYSTEM_MODE_COMFORT = 2,
    USER_MOTOR_SYSTEM_MODE_SPORTS = 3,
    USER_MOTOR_SYSTEM_MODE_DAMPING = 4
} User_Motor_system_mode_en;

typedef enum
{
    USER_MOTOR_TORQUE_LEVEL_UNKNOWN = 0,
    USER_MOTOR_TORQUE_LEVEL_1 = 1,
    USER_MOTOR_TORQUE_LEVEL_2 = 2,
    USER_MOTOR_TORQUE_LEVEL_3 = 3,
    USER_MOTOR_TORQUE_LEVEL_4 = 4
} User_Motor_torque_level_en;

struct User_IMU_gyro_t
{
    float x;
    float y;
    float z;
};
struct User_IMU_acc_t
{
    float x;
    float y;
    float z;
};
struct User_IMU_euler_t
{
    float roll;
    float pitch;
    float yaw;
};
struct User_IMU_quaternion_t
{
    float w;
    float x;
    float y;
    float z;
};
struct User_IMU_data_t
{
    struct User_IMU_gyro_t user_gyro;
    struct User_IMU_acc_t user_acc;
    struct User_IMU_euler_t user_euler;
    struct User_IMU_quaternion_t user_quaternion;
};
struct User_Barometer_data_t
{
    float pressure_hpa;          //气压，单位hPa
    float altitude_m;            //海拔，单位m
    float temperature_deg_c;     //气压计温度，单位摄氏度
    uint8_t altitude_ready;      //海拔数据有效标志
};
struct User_Motor_feedback_t
{
    uint16_t fault_code;         //电机故障码
    uint8_t power_status;        //电机系统电源状态原始值
    volatile User_Motor_system_mode_en system_mode; //电机当前系统模式，0未知，1休息，2舒适，3运动，4阻尼
    User_Motor_torque_level_en torque_level; //电机当前力矩挡位，0未知，1至4对应第一至第四级
    float input_position;        //电机输入端位置
    float output_position;       //电机输出端位置
    float torque;                //电机实际力矩
    float current;               //电机实际电流
    float input_velocity;        //电机输入端速度
    float input_acceleration;    //电机输入端加速度
    float output_velocity;       //电机输出端速度
    float output_acceleration;   //电机输出端加速度
    float temperature_deg_c;     //电机温度，单位摄氏度
};
struct User_Motor_data_t
{
    struct User_Motor_feedback_t left;
    struct User_Motor_feedback_t right;
};

/* Exported extern variables -------------------------------------------------*/
extern struct User_IMU_data_t User_IMU_Data;
extern struct User_Barometer_data_t User_Barometer_Data;
extern struct User_Motor_data_t User_Motor_Data;
extern volatile uint8_t User_Battery_Level;

/* Exported functions prototypes ---------------------------------------------*/
void User_IMU_Data_Update(void);
void User_Barometer_Data_Update(void);
void User_Motor_Data_Update(void);
void User_Battery_Level_Update(void);
int32_t User_UART_Send(const float *channel_data, uint16_t channel_count);
int32_t User_Voice_Enqueue(uint8_t voice_id);
int32_t User_LED_On(void);
int32_t User_LED_Off(void);
int32_t User_LED_Blink(void);
int32_t User_LED_Blink_Time_Set(uint16_t blink_time_ms);
int32_t User_Motor_Mode_Set(User_Motor_direction_en direction,
                            User_Motor_control_mode_en mode);
int32_t User_Motor_MIT_Mode_Set(User_Motor_direction_en direction);
int32_t User_Motor_Enable(User_Motor_direction_en direction);
int32_t User_Motor_Disable(User_Motor_direction_en direction);
int32_t User_Motor_Torque_Set(User_Motor_direction_en direction, float torque);
int32_t User_Motor_Velocity_Set(User_Motor_direction_en direction, float velocity);
int32_t User_Motor_Position_Set(User_Motor_direction_en direction, float position);
int32_t User_Motor_MIT_Set(User_Motor_direction_en direction,
                           float position,
                           float velocity,
                           float kp,
                           float kd,
                           float torque);

#ifdef __cplusplus
}
#endif
#endif   /* __USER_DATA_H__ */
