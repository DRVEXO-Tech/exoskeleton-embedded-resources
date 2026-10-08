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
#define USER_LED_ON_HOLD_TIME_MS       2000U
#define USER_LED_OFF_HOLD_TIME_MS      2000U
#define USER_LED_BLINK_INTERVAL_MS      500U

/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/

/**
* @brief 用户二次开发主函数，系统就绪后进入并持续运行
* @param 无
* @return 无
*/
void User_main(void)
{
    /* LED保持常亮2秒。 */
    User_LED_On();
    HAL_Delay(USER_LED_ON_HOLD_TIME_MS);

    /* LED保持常灭2秒。 */
    User_LED_Off();
    HAL_Delay(USER_LED_OFF_HOLD_TIME_MS);

    /* LED每500毫秒翻转一次，并由后台任务持续闪烁。 */
    User_LED_Blink_Time_Set(USER_LED_BLINK_INTERVAL_MS);
    User_LED_Blink();

    while (1)
    {
        /* LED闪烁由后台任务执行，用户主循环无需重复调用。 */
    }
}
