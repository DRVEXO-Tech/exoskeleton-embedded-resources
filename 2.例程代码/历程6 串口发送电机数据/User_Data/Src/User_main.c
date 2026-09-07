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
    float send_data[20] = {0.0f};

    while (1)
    {
        /* 先发送左电机完整反馈，再按相同字段顺序发送右电机完整反馈。 */
        send_data[0] = (float)User_Motor_Data.left.fault_code;
        send_data[1] = User_Motor_Data.left.input_position;
        send_data[2] = User_Motor_Data.left.output_position;
        send_data[3] = User_Motor_Data.left.torque;
        send_data[4] = User_Motor_Data.left.current;
        send_data[5] = User_Motor_Data.left.input_velocity;
        send_data[6] = User_Motor_Data.left.input_acceleration;
        send_data[7] = User_Motor_Data.left.output_velocity;
        send_data[8] = User_Motor_Data.left.output_acceleration;
        send_data[9] = User_Motor_Data.left.temperature_deg_c;
        send_data[10] = (float)User_Motor_Data.right.fault_code;
        send_data[11] = User_Motor_Data.right.input_position;
        send_data[12] = User_Motor_Data.right.output_position;
        send_data[13] = User_Motor_Data.right.torque;
        send_data[14] = User_Motor_Data.right.current;
        send_data[15] = User_Motor_Data.right.input_velocity;
        send_data[16] = User_Motor_Data.right.input_acceleration;
        send_data[17] = User_Motor_Data.right.output_velocity;
        send_data[18] = User_Motor_Data.right.output_acceleration;
        send_data[19] = User_Motor_Data.right.temperature_deg_c;
        User_UART_Send(send_data, 20U);
        HAL_Delay(100U);
    }
}
