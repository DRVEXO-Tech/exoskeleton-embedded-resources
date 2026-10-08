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
    float send_data[4];

	while (1)
    {
        send_data[0] = (float)User_Motor_Data.left.system_mode;
        send_data[1] = (float)User_Motor_Data.right.system_mode;
        send_data[2] = (float)User_Motor_Data.left.torque_level;
        send_data[3] = (float)User_Motor_Data.right.torque_level;

        User_UART_Send(send_data, 4U);
        HAL_Delay(100U);
    }
}
