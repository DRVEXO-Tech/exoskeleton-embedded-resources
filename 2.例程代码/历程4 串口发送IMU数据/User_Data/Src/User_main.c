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
    float send_data[13] = {0.0f};

    while (1)
    {
        /* 按角速度、加速度、欧拉角和四元数顺序发送后台更新的IMU数据。 */
        send_data[0] = User_IMU_Data.user_gyro.x;
        send_data[1] = User_IMU_Data.user_gyro.y;
        send_data[2] = User_IMU_Data.user_gyro.z;
        send_data[3] = User_IMU_Data.user_acc.x;
        send_data[4] = User_IMU_Data.user_acc.y;
        send_data[5] = User_IMU_Data.user_acc.z;
        send_data[6] = User_IMU_Data.user_euler.roll;
        send_data[7] = User_IMU_Data.user_euler.pitch;
        send_data[8] = User_IMU_Data.user_euler.yaw;
        send_data[9] = User_IMU_Data.user_quaternion.w;
        send_data[10] = User_IMU_Data.user_quaternion.x;
        send_data[11] = User_IMU_Data.user_quaternion.y;
        send_data[12] = User_IMU_Data.user_quaternion.z;
        User_UART_Send(send_data, 13U);
        HAL_Delay(100U);
    }
}
