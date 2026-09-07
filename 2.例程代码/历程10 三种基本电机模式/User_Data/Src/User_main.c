/**
  ******************************************************************************
  * @file           : User_main.c
  * @brief          : 用户二次开发主入口文件
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
#include "User_main.h"

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/

/**
* @brief 用户二次开发主函数，系统就绪后进入并持续运行
* @param 无
* @return 无
*/
void User_main(void)
{
    /* 先设置速度模式，再使能左右电机。取消注释时请注释下方其他模式代码。 */
//    User_Motor_Mode_Set(USER_MOTOR_LEFT, USER_MOTOR_CONTROL_MODE_VELOCITY);
//    User_Motor_Enable(USER_MOTOR_LEFT);
//    User_Motor_Mode_Set(USER_MOTOR_RIGHT, USER_MOTOR_CONTROL_MODE_VELOCITY);
//    User_Motor_Enable(USER_MOTOR_RIGHT);
//    HAL_Delay(50U);

    /* 先设置位置模式，再使能左右电机。取消注释时请注释下方力矩模式代码。 */
//    User_Motor_Mode_Set(USER_MOTOR_LEFT, USER_MOTOR_CONTROL_MODE_POSITION);
//    User_Motor_Enable(USER_MOTOR_LEFT);
//    User_Motor_Mode_Set(USER_MOTOR_RIGHT, USER_MOTOR_CONTROL_MODE_POSITION);
//    User_Motor_Enable(USER_MOTOR_RIGHT);
//    HAL_Delay(50U);

    /* 先设置力矩模式，再使能左右电机。 */
    User_Motor_Mode_Set(USER_MOTOR_LEFT, USER_MOTOR_CONTROL_MODE_TORQUE);
    User_Motor_Enable(USER_MOTOR_LEFT);
    User_Motor_Mode_Set(USER_MOTOR_RIGHT, USER_MOTOR_CONTROL_MODE_TORQUE);
    User_Motor_Enable(USER_MOTOR_RIGHT);
    HAL_Delay(50U);

    while (1)
    {
        /* 使左右电机以10.0 rad/s目标速度同向转动。 */
//        User_Motor_Velocity_Set(USER_MOTOR_LEFT, 10.0f);
//        User_Motor_Velocity_Set(USER_MOTOR_RIGHT, 10.0f);

        /* 左右电机每隔1000毫秒切换到下一个目标位置。 */
//        User_Motor_Position_Set(USER_MOTOR_LEFT, 10.0f);
//        User_Motor_Position_Set(USER_MOTOR_RIGHT, -10.0f);
//        HAL_Delay(1000U);

//        User_Motor_Position_Set(USER_MOTOR_LEFT, -10.0f);
//        User_Motor_Position_Set(USER_MOTOR_RIGHT, 10.0f);
//        HAL_Delay(1000U);

        /* 左右电机持续发送0.3目标力矩。 */
        User_Motor_Torque_Set(USER_MOTOR_LEFT, 0.2f);
        User_Motor_Torque_Set(USER_MOTOR_RIGHT, 0.2f);
        HAL_Delay(10U);
    }
}
