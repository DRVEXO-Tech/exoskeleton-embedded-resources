/**
  ******************************************************************************
  * @file           : PID.c
  * @brief          : PID控制文件
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
#include "PID.h"
#include <string.h>

/* Define ------------------------------------------------------------------*/
#define TEMPERATURE_PID_KP 3.0f
#define TEMPERATURE_PID_KI 0.2f
#define TEMPERATURE_PID_KD_EX 0.0f
#define TEMPERATURE_PID_KD_FB 0.0f
#define TEMPERATURE_PID_INTE_D_LIM 30.0f
#define TEMPERATURE_PID_INTE_LIM 50.0f
#define TEMPERATURE_PID_MIN_SYS_TORQUE 25U
#define TEMPERATURE_PID_MAX_SYS_TORQUE 100U

/* Variable ------------------------------------------------------------------*/
PID_t Temperature_PID;
static uint8_t temperature_pid_init_flag = 0U;

/* Function ------------------------------------------------------------------*/

/**
* @brief 初始化温度PID参数和状态
* @param 无
* @return 无
*/
void Temperature_PID_Init(void)
{
    memset(&Temperature_PID, 0, sizeof(Temperature_PID));

    Temperature_PID.PID_Para.kp = TEMPERATURE_PID_KP;
    Temperature_PID.PID_Para.ki = TEMPERATURE_PID_KI;
    Temperature_PID.PID_Para.kd_ex = TEMPERATURE_PID_KD_EX;
    Temperature_PID.PID_Para.kd_fb = TEMPERATURE_PID_KD_FB;
    Temperature_PID.PID_Para.k_ff = 0.0f;
    Temperature_PID.PID_Para.fb_d_mode = 0U;
    temperature_pid_init_flag = 1U;
}

/**
* @brief 计算PID输出
* @param dT_ms 控制周期，单位毫秒
* @param in_ff 前馈输入
* @param expect 期望值
* @param feedback 反馈值
* @param PID PID控制器
* @param inte_d_lim 单次积分误差限幅
* @param inte_lim 积分总量限幅
* @return PID输出
*/
float PID_calculate(float dT_ms,
                    float in_ff,
                    float expect,
                    float feedback,
                    PID_t *PID,
                    float inte_d_lim,
                    float inte_lim)
{
    float differential;
    float dt_s;
    float hz;

    if(PID == NULL)
    {
        return 0.0f;
    }

    if(dT_ms <= 0.0f)
    {
        dT_ms = 1.0f;
    }

    dt_s = dT_ms * 0.001f;
    hz = 1000.0f / dT_ms;
    PID->PID_val.exp_d = (expect - PID->PID_val.exp_old) * hz;

    if(PID->PID_Para.fb_d_mode == 0U)
    {
        PID->PID_val.fb_d = (feedback - PID->PID_val.feedback_old) * hz;
    }
    else
    {
        PID->PID_val.fb_d = PID->PID_val.fb_d_ex;
    }

    differential = (PID->PID_Para.kd_ex * PID->PID_val.exp_d) -
                   (PID->PID_Para.kd_fb * PID->PID_val.fb_d);

    PID->PID_val.err = expect - feedback;
    PID->PID_val.ff = in_ff;
    PID->PID_val.err_i += PID->PID_Para.ki *
                          PID_LIMIT(PID->PID_val.err, -inte_d_lim, inte_d_lim) *
                          dt_s;
    PID->PID_val.err_i = PID_LIMIT(PID->PID_val.err_i, -inte_lim, inte_lim);

    PID->PID_val.out = (PID->PID_Para.k_ff * in_ff) +
                       (PID->PID_Para.kp * PID->PID_val.err) +
                       differential +
                       PID->PID_val.err_i;

    PID->PID_val.feedback_old = feedback;
    PID->PID_val.exp_old = expect;

    return PID->PID_val.out;
}

/**
* @brief 根据电机温度用Temperature_PID限制系统力矩档位
 * @param start_temperature 启动限温的温度
* @param sys_torque 当前系统力矩档位
* @param left_temperature 左电机温度
* @param right_temperature 右电机温度
* @param dT_ms 控制周期，单位毫秒
* @return 限制后的系统力矩档位
*/
uint8_t Temperature_PID_LimitSysTorque(float start_temperature,
                                       uint8_t sys_torque,
                                       float left_temperature,
                                       float right_temperature,
                                       float dT_ms)
{
    float motor_temperature;
    float thermal_err;
    float thermal_reduce;

    if(temperature_pid_init_flag == 0U)
    {
        Temperature_PID_Init();
    }

    if(sys_torque < TEMPERATURE_PID_MIN_SYS_TORQUE)
    {
        sys_torque = TEMPERATURE_PID_MIN_SYS_TORQUE;
    }
    else if(sys_torque > TEMPERATURE_PID_MAX_SYS_TORQUE)
    {
        sys_torque = TEMPERATURE_PID_MAX_SYS_TORQUE;
    }

    motor_temperature = left_temperature;
    if(right_temperature > motor_temperature)
    {
        motor_temperature = right_temperature;
    }

    thermal_err = motor_temperature - start_temperature;
    thermal_reduce = PID_calculate(dT_ms,
                                   0.0f,
                                   thermal_err,
                                   0.0f,
                                   &Temperature_PID,
                                   TEMPERATURE_PID_INTE_D_LIM,
                                   TEMPERATURE_PID_INTE_LIM);
    if(thermal_reduce <= 0.0f)
    {
        memset(&Temperature_PID.PID_val, 0, sizeof(Temperature_PID.PID_val));
        thermal_reduce = 0.0f;
    }
    thermal_reduce = PID_LIMIT(thermal_reduce,
                               0.0f,
                               (float)(sys_torque - TEMPERATURE_PID_MIN_SYS_TORQUE));
    sys_torque = (uint8_t)((float)sys_torque - thermal_reduce);
    if(sys_torque < TEMPERATURE_PID_MIN_SYS_TORQUE)
    {
        sys_torque = TEMPERATURE_PID_MIN_SYS_TORQUE;
    }

    return sys_torque;
}
