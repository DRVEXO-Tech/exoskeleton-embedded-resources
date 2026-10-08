/**
  ******************************************************************************
  * @file           : User_Data.c
  * @brief          : 用户数据文件
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
#include "User_Data.h"
#include "DRVEXO_IMU.h"
#include "APP_LPS27HHTW.h"
#include "APP_FDCAN.h"
#include "APP_VOICE.h"
#include "APP_LED.h"
#include "APP_Control.h"
#include "DRVEXO_Scheduler.h"
#include "Platform_UART.h"
#include "Sys.h"

/* Define --------------------------------------------------------------------*/
#define USER_UART_JUSTFLOAT_TAIL_SIZE    4U
#define USER_UART_JUSTFLOAT_TAIL_0       0x00U
#define USER_UART_JUSTFLOAT_TAIL_1       0x00U
#define USER_UART_JUSTFLOAT_TAIL_2       0x80U
#define USER_UART_JUSTFLOAT_TAIL_3       0x7FU
#define USER_MOTOR_RAW_MODE_COMFORT      0U
#define USER_MOTOR_RAW_MODE_SPORTS       2U
#define USER_MOTOR_RAW_MODE_DAMPING      4U
#define USER_MOTOR_RAW_MODE_REST         5U
#define USER_MOTOR_RAW_TORQUE_LEVEL_1    25U
#define USER_MOTOR_RAW_TORQUE_LEVEL_2    50U
#define USER_MOTOR_RAW_TORQUE_LEVEL_3    75U
#define USER_MOTOR_RAW_TORQUE_LEVEL_4    100U

#pragma pack(push, 1)
typedef union
{
    struct
    {
        float channel[USER_UART_JUSTFLOAT_MAX_CHANNELS];
    } data;
    uint8_t buf[(USER_UART_JUSTFLOAT_MAX_CHANNELS * sizeof(float)) +
                USER_UART_JUSTFLOAT_TAIL_SIZE];
} User_UART_JustFloat_frame_u;
#pragma pack(pop)

/* Variable ------------------------------------------------------------------*/

static User_UART_JustFloat_frame_u user_uart_justfloat_frame;

struct User_IMU_data_t User_IMU_Data = {
    .user_gyro = {0.0f, 0.0f, 0.0f},
    .user_acc = {0.0f, 0.0f, 0.0f},
    .user_euler = {0.0f, 0.0f, 0.0f},
    .user_quaternion = {1.0f, 0.0f, 0.0f, 0.0f}
};
struct User_Barometer_data_t User_Barometer_Data = {
    .pressure_hpa = 0.0f,
    .altitude_m = 0.0f,
    .temperature_deg_c = 0.0f,
    .altitude_ready = 0U
};
struct User_Motor_data_t User_Motor_Data = {
    .left = {
        .system_mode = USER_MOTOR_SYSTEM_MODE_UNKNOWN,
        .torque_level = USER_MOTOR_TORQUE_LEVEL_UNKNOWN
    },
    .right = {
        .system_mode = USER_MOTOR_SYSTEM_MODE_UNKNOWN,
        .torque_level = USER_MOTOR_TORQUE_LEVEL_UNKNOWN
    }
};
volatile uint8_t User_Battery_Level = 0U;

/* Function ------------------------------------------------------------------*/

/**
* @brief 将电机系统模式原始编码转换为用户连续编号
* @param raw_mode 电机反馈中的系统模式原始编码
* @return 用户系统模式编号
*/
static User_Motor_system_mode_en User_Motor_System_Mode_Convert(uint8_t raw_mode)
{
    switch (raw_mode)
    {
    case USER_MOTOR_RAW_MODE_COMFORT:
        return USER_MOTOR_SYSTEM_MODE_COMFORT;
    case USER_MOTOR_RAW_MODE_SPORTS:
        return USER_MOTOR_SYSTEM_MODE_SPORTS;
    case USER_MOTOR_RAW_MODE_DAMPING:
        return USER_MOTOR_SYSTEM_MODE_DAMPING;
    case USER_MOTOR_RAW_MODE_REST:
        return USER_MOTOR_SYSTEM_MODE_REST;
    default:
        return USER_MOTOR_SYSTEM_MODE_UNKNOWN;
    }
}

/**
* @brief 将电机力矩挡位原始编码转换为用户连续编号
* @param raw_level 电机反馈中的力矩挡位原始编码
* @return 用户力矩挡位编号
*/
static User_Motor_torque_level_en User_Motor_Torque_Level_Convert(uint16_t raw_level)
{
    switch (raw_level)
    {
    case USER_MOTOR_RAW_TORQUE_LEVEL_1:
        return USER_MOTOR_TORQUE_LEVEL_1;
    case USER_MOTOR_RAW_TORQUE_LEVEL_2:
        return USER_MOTOR_TORQUE_LEVEL_2;
    case USER_MOTOR_RAW_TORQUE_LEVEL_3:
        return USER_MOTOR_TORQUE_LEVEL_3;
    case USER_MOTOR_RAW_TORQUE_LEVEL_4:
        return USER_MOTOR_TORQUE_LEVEL_4;
    default:
        return USER_MOTOR_TORQUE_LEVEL_UNKNOWN;
    }
}

/**
* @brief 刷新用户IMU数据快照
* @param 无
* @return 无
*/
void User_IMU_Data_Update(void)
{
    DRVEXOQuaternion quaternion = DRVEXOAhrsGetQuaternion(&ahrs);

    User_IMU_Data.user_gyro.x = IMU_Data_IEM[IMU1].IMU_GRYO_dps[0];
    User_IMU_Data.user_gyro.y = IMU_Data_IEM[IMU1].IMU_GRYO_dps[1];
    User_IMU_Data.user_gyro.z = IMU_Data_IEM[IMU1].IMU_GRYO_dps[2];

    User_IMU_Data.user_acc.x = IMU_Data_IEM[IMU1].IMU_ACC_g[0];
    User_IMU_Data.user_acc.y = IMU_Data_IEM[IMU1].IMU_ACC_g[1];
    User_IMU_Data.user_acc.z = IMU_Data_IEM[IMU1].IMU_ACC_g[2];

    User_IMU_Data.user_euler.roll = euler.angle.roll;
    User_IMU_Data.user_euler.pitch = euler.angle.pitch;
    User_IMU_Data.user_euler.yaw = euler.angle.yaw;

    User_IMU_Data.user_quaternion.w = quaternion.element.w;
    User_IMU_Data.user_quaternion.x = quaternion.element.x;
    User_IMU_Data.user_quaternion.y = quaternion.element.y;
    User_IMU_Data.user_quaternion.z = quaternion.element.z;
}

/**
* @brief 刷新用户气压计数据快照
* @param 无
* @return 无
*/
void User_Barometer_Data_Update(void)
{
    User_Barometer_Data.pressure_hpa = pressure_hPa;
    User_Barometer_Data.altitude_m = altitude_m;
    User_Barometer_Data.temperature_deg_c = temperature_degC_LPS27;
    User_Barometer_Data.altitude_ready = barometer_altitude_ready;
}

/**
* @brief 刷新用户左右电机运行数据快照
* @param 无
* @return 无
*/
void User_Motor_Data_Update(void)
{
    User_Motor_Data.left.fault_code = app_fdcan_left_motor_data.data.fault_code;
    User_Motor_Data.left.power_status = fdcan_get_syspower_flag();
    if (sys_ready.Left_Motor_Ready != 0U)
    {
        User_Motor_Data.left.system_mode =
            User_Motor_System_Mode_Convert(fdcan_get_sysmode());
    }
    User_Motor_Data.left.torque_level =
        User_Motor_Torque_Level_Convert(fdcan_get_left_motor_sys_torque());
    User_Motor_Data.left.input_position = app_fdcan_left_motor_data.data.input_angle;
    User_Motor_Data.left.output_position = app_fdcan_left_motor_data.data.output_angle;
    User_Motor_Data.left.torque = app_fdcan_left_motor_data.data.torque;
    User_Motor_Data.left.current = app_fdcan_left_motor_data.data.current;
    User_Motor_Data.left.input_velocity = app_fdcan_left_motor_data.data.input_velocity;
    User_Motor_Data.left.input_acceleration = app_fdcan_left_motor_data.data.input_acceleration;
    User_Motor_Data.left.output_velocity = app_fdcan_left_motor_data.data.output_velocity;
    User_Motor_Data.left.output_acceleration = app_fdcan_left_motor_data.data.output_acceleration;
    User_Motor_Data.left.temperature_deg_c = app_fdcan_left_motor_data.data.temperature;

    User_Motor_Data.right.fault_code = app_fdcan_right_motor_data.data.fault_code;
    User_Motor_Data.right.power_status = fdcan_get_syspower_flag_r();
    if (sys_ready.Right_Motor_Ready != 0U)
    {
        User_Motor_Data.right.system_mode =
            User_Motor_System_Mode_Convert(fdcan_get_sysmode_r());
    }
    User_Motor_Data.right.torque_level =
        User_Motor_Torque_Level_Convert(fdcan_get_right_motor_sys_torque());
    User_Motor_Data.right.input_position = app_fdcan_right_motor_data.data.input_angle;
    User_Motor_Data.right.output_position = app_fdcan_right_motor_data.data.output_angle;
    User_Motor_Data.right.torque = app_fdcan_right_motor_data.data.torque;
    User_Motor_Data.right.current = app_fdcan_right_motor_data.data.current;
    User_Motor_Data.right.input_velocity = app_fdcan_right_motor_data.data.input_velocity;
    User_Motor_Data.right.input_acceleration = app_fdcan_right_motor_data.data.input_acceleration;
    User_Motor_Data.right.output_velocity = app_fdcan_right_motor_data.data.output_velocity;
    User_Motor_Data.right.output_acceleration = app_fdcan_right_motor_data.data.output_acceleration;
    User_Motor_Data.right.temperature_deg_c = app_fdcan_right_motor_data.data.temperature;
}

/**
* @brief 刷新用户电池剩余电量
* @param 无
* @return 无
*/
void User_Battery_Level_Update(void)
{
    User_Battery_Level = app_fdcan_battery_data.data.bat_capacity;
}

/**
* @brief 使用USART1 DMA发送一帧VOFA+ JustFloat浮点通道数据
* @param channel_data 用户浮点通道数组
* @param channel_count 浮点通道数量，范围为1至255
* @return 0表示发送启动成功，负值表示参数无效、DMA忙或发送失败
*/
int32_t User_UART_Send(const float *channel_data, uint16_t channel_count)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)platform_uart_user_dev.handle;
    uint16_t payload_size;
    uint16_t frame_size;
    uint16_t i;
    int32_t ret;

    if((channel_data == NULL) ||
       (channel_count == 0U) ||
       (channel_count > USER_UART_JUSTFLOAT_MAX_CHANNELS))
    {
        sys_err.User_UART_tx_err_cnt++;
        return -LY_ERR_INVALID;
    }
    if((huart == NULL) || (platform_uart_user_dev.write == NULL))
    {
        sys_err.User_UART_tx_err_cnt++;
        return -LY_ERR_NOT_READY;
    }
    if(HAL_UART_GetState(huart) != HAL_UART_STATE_READY)
    {
        sys_err.User_UART_tx_err_cnt++;
        return -LY_ERR_BUSY;
    }

    for(i = 0U; i < channel_count; i++)
    {
        user_uart_justfloat_frame.data.channel[i] = channel_data[i];
    }
    payload_size = (uint16_t)(channel_count * sizeof(float));
    user_uart_justfloat_frame.buf[payload_size] = USER_UART_JUSTFLOAT_TAIL_0;
    user_uart_justfloat_frame.buf[payload_size + 1U] = USER_UART_JUSTFLOAT_TAIL_1;
    user_uart_justfloat_frame.buf[payload_size + 2U] = USER_UART_JUSTFLOAT_TAIL_2;
    user_uart_justfloat_frame.buf[payload_size + 3U] = USER_UART_JUSTFLOAT_TAIL_3;
    frame_size = (uint16_t)(payload_size + USER_UART_JUSTFLOAT_TAIL_SIZE);

    ret = platform_uart_user_dev.write(&platform_uart_user_dev,
                                       0U,
                                       user_uart_justfloat_frame.buf,
                                       frame_size);
    if(ret < 0)
    {
        sys_err.User_UART_tx_err_cnt++;
    }
    return ret;
}

/**
* @brief 将指定语音加入APP语音播放队列
* @param voice_id 语音编号，不能为0
* @return 0表示入队成功，负值表示语音编号无效或队列已满
*/
int32_t User_Voice_Enqueue(uint8_t voice_id)
{
    int32_t ret = app_voice_enqueue(voice_id);

    if (ret < 0)
    {
        sys_err.Voice_Play_err_cnt++;
    }
    return ret;
}

/**
* @brief 设置用户LED常亮
* @param 无
* @return 0表示设置成功，负值表示LED控制失败
*/
int32_t User_LED_On(void)
{
    int32_t ret = APP_LED_Mode_Set(APP_LED_MODE_ON);

    if(ret < 0)
    {
        sys_err.LED_Task_err_cnt++;
    }
    return ret;
}

/**
* @brief 设置用户LED常灭
* @param 无
* @return 0表示设置成功，负值表示LED控制失败
*/
int32_t User_LED_Off(void)
{
    int32_t ret = APP_LED_Mode_Set(APP_LED_MODE_OFF);

    if(ret < 0)
    {
        sys_err.LED_Task_err_cnt++;
    }
    return ret;
}

/**
* @brief 设置用户LED进入闪烁模式
* @param 无
* @return 0表示设置成功，负值表示LED任务尚未初始化
*/
int32_t User_LED_Blink(void)
{
    int32_t ret;

    if(LEDHandle == NULL)
    {
        sys_err.LED_Task_err_cnt++;
        return -LY_ERR_NOT_READY;
    }
    ret = APP_LED_Mode_Set(APP_LED_MODE_BLINK);
    if(ret < 0)
    {
        sys_err.LED_Task_err_cnt++;
        return ret;
    }
    LEDHandle->ulLastRunTime = GetTick();
    return LY_OK;
}

/**
* @brief 设置LED亮灭翻转时间
* @param blink_time_ms 每次亮灭翻转的间隔时间，单位ms，不能为0
* @return 0表示设置成功，负值表示参数无效或LED任务未初始化
*/
int32_t User_LED_Blink_Time_Set(uint16_t blink_time_ms)
{
    if (blink_time_ms == 0U)
    {
        sys_err.LED_Task_err_cnt++;
        return -LY_ERR_INVALID;
    }
    if (LEDHandle == NULL)
    {
        sys_err.LED_Task_err_cnt++;
        return -LY_ERR_NOT_READY;
    }

    LEDHandle->usRateMs = blink_time_ms;
    LEDHandle->ulLastRunTime = GetTick();
    return LY_OK;
}

/**
* @brief 设置指定电机的力矩、速度或位置控制模式
* @param direction 用户层电机方向
* @param mode 用户层电机控制模式
* @return 0表示模式设置成功，负值表示参数无效、发送失败或响应超时
*/
int32_t User_Motor_Mode_Set(User_Motor_direction_en direction,
                            User_Motor_control_mode_en mode)
{
    Mode_en app_mode;
    int32_t ret;

    if ((direction != USER_MOTOR_LEFT) && (direction != USER_MOTOR_RIGHT))
    {
        sys_err.Motor_Mode_Set_err_cnt++;
        return -LY_ERR_INVALID;
    }

    switch (mode)
    {
    case USER_MOTOR_CONTROL_MODE_TORQUE:
        app_mode = CONTROL_MODE_TORQUE_RAMP;
        break;
    case USER_MOTOR_CONTROL_MODE_VELOCITY:
        app_mode = CONTROL_MODE_VELOCITY_RAMP;
        break;
    case USER_MOTOR_CONTROL_MODE_POSITION:
        app_mode = CONTROL_MODE_POSITION_FILTER;
        break;
    default:
        sys_err.Motor_Mode_Set_err_cnt++;
        return -LY_ERR_INVALID;
    }

    ret = APP_Control_SetMode((Direction_en)direction,
                              app_mode,
                              USER_MOTOR_COMMAND_TIMEOUT_MS);
    if (ret < 0)
    {
        sys_err.Motor_Mode_Set_err_cnt++;
    }
    return ret;
}

/**
* @brief 将指定电机设置为MIT模式
* @param direction 用户层电机方向
* @return 0表示模式设置成功，负值表示参数无效、发送失败或响应超时
*/
int32_t User_Motor_MIT_Mode_Set(User_Motor_direction_en direction)
{
    int32_t ret;

    if ((direction != USER_MOTOR_LEFT) && (direction != USER_MOTOR_RIGHT))
    {
        sys_err.Motor_Mode_Set_err_cnt++;
        return -LY_ERR_INVALID;
    }

    ret = APP_Control_SetMode((Direction_en)direction,
                              CONTROL_MODE_MIT,
                              USER_MOTOR_COMMAND_TIMEOUT_MS);
    if (ret < 0)
    {
        sys_err.Motor_Mode_Set_err_cnt++;
    }
    return ret;
}

/**
* @brief 使能指定电机
* @param direction 用户层电机方向
* @return 0表示使能成功，负值表示参数无效、发送失败或响应超时
*/
int32_t User_Motor_Enable(User_Motor_direction_en direction)
{
    int32_t ret;

    if ((direction != USER_MOTOR_LEFT) && (direction != USER_MOTOR_RIGHT))
    {
        sys_err.Motor_Enable_err_cnt++;
        return -LY_ERR_INVALID;
    }

    ret = APP_Control_Enable((Direction_en)direction, USER_MOTOR_COMMAND_TIMEOUT_MS);
    if (ret < 0)
    {
        sys_err.Motor_Enable_err_cnt++;
    }
    return ret;
}

/**
* @brief 失能指定电机
* @param direction 用户层电机方向
* @return 0表示失能成功，负值表示参数无效、发送失败或响应超时
*/
int32_t User_Motor_Disable(User_Motor_direction_en direction)
{
    int32_t ret;

    if ((direction != USER_MOTOR_LEFT) && (direction != USER_MOTOR_RIGHT))
    {
        sys_err.Motor_Disable_err_cnt++;
        return -LY_ERR_INVALID;
    }

    ret = APP_Control_Disable((Direction_en)direction, USER_MOTOR_COMMAND_TIMEOUT_MS);
    if (ret < 0)
    {
        sys_err.Motor_Disable_err_cnt++;
    }
    return ret;
}

/**
* @brief 设置指定电机的目标力矩
* @param direction 用户层电机方向
* @param torque 目标力矩，单位由电机通信协议定义
* @return 0表示发送成功，负值表示参数无效或发送失败
*/
int32_t User_Motor_Torque_Set(User_Motor_direction_en direction, float torque)
{
    int32_t ret;

    if ((direction != USER_MOTOR_LEFT) && (direction != USER_MOTOR_RIGHT))
    {
        sys_err.Motor_Control_err_cnt++;
        return -LY_ERR_INVALID;
    }

    ret = APP_Control_SetTorque((Direction_en)direction, torque);
    if (ret < 0)
    {
        sys_err.Motor_Control_err_cnt++;
    }
    return ret;
}

/**
* @brief 设置指定电机的目标速度
* @param direction 用户层电机方向
* @param velocity 目标速度，单位rad/s
* @return 0表示发送成功，负值表示参数无效或发送失败
*/
int32_t User_Motor_Velocity_Set(User_Motor_direction_en direction, float velocity)
{
    int32_t ret;

    if ((direction != USER_MOTOR_LEFT) && (direction != USER_MOTOR_RIGHT))
    {
        sys_err.Motor_Control_err_cnt++;
        return -LY_ERR_INVALID;
    }

    ret = APP_Control_SetVelocity((Direction_en)direction, velocity);
    if (ret < 0)
    {
        sys_err.Motor_Control_err_cnt++;
    }
    return ret;
}

/**
* @brief 设置指定电机的目标位置
* @param direction 用户层电机方向
* @param position 目标位置，单位rad
* @return 0表示发送成功，负值表示参数无效或发送失败
*/
int32_t User_Motor_Position_Set(User_Motor_direction_en direction, float position)
{
    int32_t ret;

    if ((direction != USER_MOTOR_LEFT) && (direction != USER_MOTOR_RIGHT))
    {
        sys_err.Motor_Control_err_cnt++;
        return -LY_ERR_INVALID;
    }

    ret = APP_Control_SetPosition((Direction_en)direction, position);
    if (ret < 0)
    {
        sys_err.Motor_Control_err_cnt++;
    }
    return ret;
}

/**
* @brief 设置指定电机的MIT控制参数
* @param direction 用户层电机方向
* @param position 目标位置
* @param velocity 目标速度
* @param kp 位置增益
* @param kd 速度增益
* @param torque 前馈力矩
* @return 0表示发送成功，负值表示参数无效或发送失败
*/
int32_t User_Motor_MIT_Set(User_Motor_direction_en direction,
                           float position,
                           float velocity,
                           float kp,
                           float kd,
                           float torque)
{
    int32_t ret;

    if ((direction != USER_MOTOR_LEFT) && (direction != USER_MOTOR_RIGHT))
    {
        sys_err.Motor_Control_err_cnt++;
        return -LY_ERR_INVALID;
    }

    ret = APP_Control_SetMITMode((Direction_en)direction,
                                 position,
                                 velocity,
                                 kp,
                                 kd,
                                 torque);
    if (ret < 0)
    {
        sys_err.Motor_Control_err_cnt++;
    }
    return ret;
}
