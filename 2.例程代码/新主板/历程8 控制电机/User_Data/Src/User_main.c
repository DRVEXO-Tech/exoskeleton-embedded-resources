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
    float send_data[2] = {0.0f, 0.0f};

    /* 先设置MIT模式，再使能左电机。 */
    User_Motor_MIT_Mode_Set(USER_MOTOR_LEFT);
    User_Motor_Enable(USER_MOTOR_LEFT);
    User_Motor_MIT_Mode_Set(USER_MOTOR_RIGHT);
    User_Motor_Enable(USER_MOTOR_RIGHT);
	HAL_Delay(50);
	//使电机以比较小的力矩慢速转动
    while (1)
    {
		User_Motor_MIT_Set(USER_MOTOR_LEFT, 0.0f, 0.0, 0.0f, 0.2f, 1.0f);
		User_Motor_MIT_Set(USER_MOTOR_RIGHT, 0.0f, 0.0, 0.0f, 0.2f, -1.0f);
		HAL_Delay(10);
    }
}
