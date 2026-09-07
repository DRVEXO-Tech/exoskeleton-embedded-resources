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
    float send_data[4] = {0.0f};

    while (1)
    {
        /* 按气压、海拔、温度和海拔有效标志顺序发送后台更新的气压计数据。 */
        send_data[0] = User_Barometer_Data.pressure_hpa;
        send_data[1] = User_Barometer_Data.altitude_m;
        send_data[2] = User_Barometer_Data.temperature_deg_c;
        send_data[3] = (float)User_Barometer_Data.altitude_ready;
        User_UART_Send(send_data, 4U);
        HAL_Delay(100U);
    }
}
