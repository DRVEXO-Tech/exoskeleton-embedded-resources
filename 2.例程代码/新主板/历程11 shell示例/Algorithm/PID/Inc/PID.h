/**
  ******************************************************************************
  * @file    PID.h
  * @brief   PID控制头文件。
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
#ifndef __PID_H__
#define __PID_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/
#define PID_LIMIT(value, min, max)    (((value) > (max)) ? (max) : (((value) < (min)) ? (min) : (value)))
#define TEMPERATURE_PID_COOL_NORMAL_START_C     45.0f
#define TEMPERATURE_PID_COOL_NORMAL_STOP_C      40.0f
#define TEMPERATURE_PID_COOL_SPORTS_START_C     50.0f
#define TEMPERATURE_PID_COOL_SPORTS_STOP_C      45.0f
#define TEMPERATURE_PID_BALANCED_NORMAL_START_C 55.0f
#define TEMPERATURE_PID_BALANCED_NORMAL_STOP_C  50.0f
#define TEMPERATURE_PID_BALANCED_SPORTS_START_C 60.0f
#define TEMPERATURE_PID_BALANCED_SPORTS_STOP_C  55.0f
#define TEMPERATURE_PID_WILD_NORMAL_START_C     65.0f
#define TEMPERATURE_PID_WILD_NORMAL_STOP_C      60.0f
#define TEMPERATURE_PID_WILD_SPORTS_START_C     70.0f
#define TEMPERATURE_PID_WILD_SPORTS_STOP_C      65.0f

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    uint8_t fb_d_mode;
    float kp;       //比例系数
    float ki;       //积分系数
    float kd_ex;    //期望微分系数
    float kd_fb;    //反馈微分系数
    float k_ff;     //前馈系数
} PID_Para_t;

typedef struct
{
    float err;
    float exp_old;
    float feedback_old;
    float fb_d;
    float fb_d_ex;
    float exp_d;
    float err_i;
    float ff;
    float pre_d;
    float out;
} PID_Val_t;

typedef struct
{
    PID_Para_t PID_Para;
    PID_Val_t PID_val;
} PID_t;

/* Exported extern variables -------------------------------------------------*/
extern PID_t Temperature_PID;

/* Exported functions prototypes ---------------------------------------------*/
void Temperature_PID_Init(void);
float PID_calculate(float dT_ms,
                    float in_ff,
                    float expect,
                    float feedback,
                    PID_t *PID,
                    float inte_d_lim,
                    float inte_lim);
uint8_t Temperature_PID_LimitSysTorque(float start_temperature,
                                       uint8_t sys_torque,
                                       float left_temperature,
                                       float right_temperature,
                                       float dT_ms);

#ifdef __cplusplus
}
#endif
#endif   /* __PID_H__ */
