/**
  ******************************************************************************
  * @file           : APP_Control.c
  * @brief          : 电机控制APP层文件
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
#include "APP_Control.h"
#include "APP_FDCAN.h"
#include "APP_ISM6DSO.h"
#include "Sys.h"
#include "config.h"

#include <string.h>

/* Define ------------------------------------------------------------------*/
#define APP_CONTROL_TEST_FAN_CMD         38U
#define APP_CONTROL_TEST_KEY_RGB_CMD     39U
#define APP_CONTROL_READ_CALIB_STATE_CMD 40U
#define APP_CONTROL_DISABLE_DISCONN_CMD  41U
#define APP_CONTROL_CALIB_START2_CMD     42U
#define APP_CONTROL_CORE_ERROR_STATUS_CMD 43U
#define APP_CONTROL_ALGORITHM_CALIB_LIGHT_CMD 44U
#define APP_CONTROL_CORE_ERROR_STATUS_LEN 2U
#define APP_CONTROL_CORE_ERROR_FLASH_MASK (1U << 0)
#define APP_CONTROL_CORE_ERROR_BARO_MASK  (1U << 1)
#define APP_CONTROL_CORE_ERROR_IMU1_MASK  (1U << 2)
#define APP_CONTROL_CORE_ERROR_IMU2_MASK  (1U << 3)

/* Variable ------------------------------------------------------------------*/
static uint8_t control_buf[24];

#pragma pack(push, 1)
typedef union
{
    struct
    {
        uint8_t error_status;
    } data;
    uint8_t buf[1];
} APP_Control_CoreErrorStatus_u;
#pragma pack(pop)

/* Function ------------------------------------------------------------------*/
/**
 * @brief 发送实时控制命令
* @param direction 电机方向
* @param len 命令长度
* @return 发送结果
*/
static int32_t app_control_send_realtime_command(Direction_en direction, uint8_t len)
{
    uint16_t target_id;

    if (direction == Left)
    {
        target_id = FDCANX_CORE_CONTROL_LEFT_ID;
    }
    else if (direction == Right)
    {
        target_id = FDCANX_CORE_CONTROL_RIGHT_ID;
    }
    else
    {
        return -LY_ERR_INVALID;
    }

    return APP_FDCAN_SendMotorControl(target_id, control_buf, len);
}

/**
* @brief 发送普通设置命令
* @param direction 电机方向
* @param len 命令长度
* @return 发送结果
*/
static int32_t app_control_send_setting_command(Direction_en direction, uint8_t len)
{
    uint16_t target_id;

    if (direction == Left)
    {
        target_id = FDCANX_CORE_SETTING_LEFT_ID;
    }
    else if (direction == Right)
    {
        target_id = FDCANX_CORE_SETTING_RIGHT_ID;
    }
    else
    {
        return -LY_ERR_INVALID;
    }

    return APP_FDCAN_SendMotorControl(target_id, control_buf, len);
}

/**
* @brief 发送控制命令并等待超时返回
* @param direction 电机方向
* @param len 命令长度
* @param Timeout 超时时间
* @return 发送结果
*/
static int32_t app_control_send_command_timeout(Direction_en direction, uint8_t len, uint32_t Timeout)
{
    uint32_t tickstart;

    if ((direction != Left) && (direction != Right))
    {
        return -LY_ERR_INVALID;
    }

    fdcan_write_receive_flag((uint8_t)direction, 1U);
    if (app_control_send_setting_command(direction, len) != LY_OK)
    {
        fdcan_write_receive_flag((uint8_t)direction, 0U);
        return -LY_ERR;
    }

    tickstart = HAL_GetTick();
    while (fdcan_get_receive_flag((uint8_t)direction) == 1)
    {
        if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != 0xFFFFFFFFU)) || (Timeout == 0U))
        {
            return -LY_ERR_TIMEOUT;
        }
    }

    return LY_OK;
}

/**
 * @brief 设置指定电机的控制模式。
 * @param direction 目标电机方向。
 * @param mode 目标控制模式。
 * @return 返回发送结果。
 */
int32_t APP_Control_SetMode(Direction_en direction, Mode_en mode, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 0U;
    control_buf[1] = (uint8_t)mode;
    return app_control_send_command_timeout(direction, 2U, Timeout);
}

/**
 * @brief 使能指定电机。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_Enable(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 1U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 关闭指定电机。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_Disable(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 2U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 设置指定电机的目标扭矩。
 * @param direction 目标电机方向。
 * @param torque 目标扭矩值。
 * @return 返回发送结果。
 */
int32_t APP_Control_SetTorque(Direction_en direction, float torque)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 3U;
    memcpy(&control_buf[1], &torque, sizeof(torque));
    return app_control_send_realtime_command(direction, 5U);
}

/**
 * @brief 设置指定电机的目标速度。
 * @param direction 目标电机方向。
 * @param velocity 目标速度值。
 * @return 返回发送结果。
 */
int32_t APP_Control_SetVelocity(Direction_en direction, float velocity)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 4U;
    memcpy(&control_buf[1], &velocity, sizeof(velocity));
    return app_control_send_realtime_command(direction, 5U);
}

/**
 * @brief 设置指定电机的目标位置。
 * @param direction 目标电机方向。
 * @param position 目标位置值。
 * @return 返回发送结果。
 */
int32_t APP_Control_SetPosition(Direction_en direction, float position)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 5U;
    memcpy(&control_buf[1], &position, sizeof(position));
    return app_control_send_realtime_command(direction, 5U);
}

/**
 * @brief 向指定电机发送同步命令。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_Sync(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 6U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 启动指定电机的标定流程。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_CalibStart(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 7U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 请求指定电机上报标定结果。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_CalibReport(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 8U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 中止指定电机的标定流程。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_CalibAbort(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 9U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 启动指定电机的抗齿槽流程。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_AnticoggingStart(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 10U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 请求指定电机上报抗齿槽结果。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_AnticoggingReport(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 11U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 中止指定电机的抗齿槽流程。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_AnticoggingAbort(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 12U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 设置指定电机当前位置为 Home。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_SetHome(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 13U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 复位指定电机错误状态。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_ErrorReset(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 14U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 请求指定电机的状态字。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_GetStatusword(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 15U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 请求指定电机主动上报状态字。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_StatuswordReport(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 16U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 请求指定电机上报 Value1。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_GetValue1(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 17U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 请求指定电机上报 Value2。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_GetValue2(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 18U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 向指定电机发送心跳命令。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_Heartbeat(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 23U;
    return app_control_send_command_timeout(direction, 2U, Timeout);
}

/**
 * @brief 设置指定电机的配置项。
 * @param direction 目标电机方向。
 * @param index 配置索引地址。
 * @param value 待写入的配置值。
 * @return 返回发送结果。
 */
int32_t APP_Control_SetConfig(Direction_en direction, int32_t index, uint32_t value, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 24U;
    memcpy(&control_buf[1], &index, sizeof(index));
    memcpy(&control_buf[5], &value, sizeof(value));
    return app_control_send_command_timeout(direction, 12U, Timeout);
}

/**
 * @brief 读取指定电机的配置项。
 * @param direction 目标电机方向。
 * @param index 配置索引地址。
 * @param value 附带的扩展参数值。
 * @return 返回发送结果。
 */
int32_t APP_Control_GetConfig(Direction_en direction, int32_t index, uint32_t value, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 25U;
    memcpy(&control_buf[1], &index, sizeof(index));
    memcpy(&control_buf[5], &value, sizeof(value));
    return app_control_send_command_timeout(direction, 12U, Timeout);
}

/**
 * @brief 保存指定电机的全部配置。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_SaveAllConfig(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 26U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 恢复指定电机的默认配置。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_ResetAllConfig(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 27U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 请求指定电机的固件版本。
 * @param direction 目标电机方向。
 * @return 返回发送结果。
 */
int32_t APP_Control_GetFwVersion(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 28U;
    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 向指定电机发送 MIT 模式控制参数。
 * @param direction 目标电机方向。
 * @param position 目标位置。
 * @param velocity 目标速度。
 * @param kp 位置环比例系数。
 * @param kd 速度环比例系数。
 * @param torque 前馈扭矩。
 * @return 返回发送结果。
 */
int32_t APP_Control_SetMITMode(Direction_en direction, float position, float velocity, float kp, float kd, float torque)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 29U;
    memcpy(&control_buf[1], &position, sizeof(position));
    memcpy(&control_buf[5], &velocity, sizeof(velocity));
    memcpy(&control_buf[9], &kp, sizeof(kp));
    memcpy(&control_buf[13], &kd, sizeof(kd));
    memcpy(&control_buf[17], &torque, sizeof(torque));
    return app_control_send_realtime_command(direction, 24U);
}

/**
 * @brief 设置指定电机的标准帧 ID。
 * @param direction 目标电机方向。
 * @param id 新的标准帧 ID。
 * @return 返回发送结果。
 */
int32_t APP_Control_SetStdId(Direction_en direction, uint16_t id, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 30U;
    memcpy(&control_buf[1], &id, sizeof(id));
    return app_control_send_command_timeout(direction, 3U, Timeout);
}







/**
 * @brief 让左右电机进入休眠流程，并按需置位语音提示标志。
 * @param 无。
 * @return 返回发送结果。
 */
int32_t APP_Control_Sleep(uint32_t Timeout)
{
    int32_t ret;

    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 31U;
    ret = app_control_send_command_timeout(Left, 1U, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    ret = app_control_send_command_timeout(Right, 1U, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }

    return LY_OK;
}

/**
 * @brief 设置灯光亮度
 * @param state 1开启 0结束
 * @return 返回发送结果。
 */
int32_t APP_Control_Set_Brightness(uint8_t Brightness,uint32_t Timeout)
{
    int32_t ret;

    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = 36;
    control_buf[1] = Brightness;
    ret = app_control_send_command_timeout(Left, 2, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    ret = app_control_send_command_timeout(Right, 2, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    return LY_OK;
}

/**
 * @brief 测试风扇
 * @param state 0停止，1开始
 * @param Timeout 超时时间
 * @return 返回发送结果。
 */
int32_t APP_Control_TestFan(uint8_t state, uint32_t Timeout)
{
    int32_t ret;

    if (state > 1U)
    {
        return -LY_ERR_INVALID;
    }

    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = APP_CONTROL_TEST_FAN_CMD;
    control_buf[1] = state;
    ret = app_control_send_command_timeout(Left, 2U, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    ret = app_control_send_command_timeout(Right, 2U, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    return LY_OK;
}

/**
 * @brief 测试按键和RGB
 * @param state 0停止，1开始
 * @param Timeout 超时时间
 * @return 返回发送结果。
 */
int32_t APP_Control_TestKeyRGB(uint8_t state, uint32_t Timeout)
{
    int32_t ret;

    if (state > 1U)
    {
        return -LY_ERR_INVALID;
    }

    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = APP_CONTROL_TEST_KEY_RGB_CMD;
    control_buf[1] = state;
    ret = app_control_send_command_timeout(Left, 2U, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    ret = app_control_send_command_timeout(Right, 2U, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    return LY_OK;
}

/**
 * @brief 读取电机校准状态
 * @param state 校准状态，0未开始，1进行中，2失败，3成功
 * @param Timeout 超时时间
 * @return 返回读取结果。
 */
int32_t APP_Control_ReadCalibrateState(uint8_t *state, uint32_t Timeout)
{
    uint8_t left_state;
    uint8_t right_state;
    int32_t ret;

    if (state == 0)
    {
        return -LY_ERR_INVALID;
    }

    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = APP_CONTROL_READ_CALIB_STATE_CMD;
    ret = app_control_send_command_timeout(Left, 1U, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    ret = fdcan_get_motor_receive_one_data((uint8_t)Left);
    if (ret < 0)
    {
        return ret;
    }
    left_state = (uint8_t)ret;

    ret = app_control_send_command_timeout(Right, 1U, Timeout);
    if (ret != LY_OK)
    {
        return ret;
    }
    ret = fdcan_get_motor_receive_one_data((uint8_t)Right);
    if (ret < 0)
    {
        return ret;
    }
    right_state = (uint8_t)ret;

    if ((left_state > 3U) || (right_state > 3U))
    {
        return -LY_ERR;
    }

    if ((left_state == 2U) || (right_state == 2U))
    {
        *state = 2U;
    }
    else if ((left_state == 1U) || (right_state == 1U))
    {
        *state = 1U;
    }
    else if ((left_state == 3U) && (right_state == 3U))
    {
        *state = 3U;
    }
    else
    {
        *state = 0U;
    }

    return LY_OK;
}

/**
 * @brief 关闭指定电机的KTM5800断线报警并保存配置。
 * @param direction 目标电机方向。
 * @param Timeout 超时时间。
 * @return 返回发送结果。
 */
int32_t APP_Control_DisableDisconnectionAlarm(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = APP_CONTROL_DISABLE_DISCONN_CMD;

    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
 * @brief 启动指定电机的KTM5800线性校准流程。
 * @param direction 目标电机方向。
 * @param Timeout 超时时间。
 * @return 返回发送结果。
 */
int32_t APP_Control_CalibStart2(Direction_en direction, uint32_t Timeout)
{
    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = APP_CONTROL_CALIB_START2_CMD;

    return app_control_send_command_timeout(direction, 1U, Timeout);
}

/**
* @brief 获取当前主控错误状态
* @param 无
* @return bit0到bit4为当前主控错误状态
*/
uint8_t APP_Control_GetCoreErrorStatus(void)
{
    uint8_t error_status = 0U;

    if ((sys_ready.Flash_Ready != 1U) || (sys_err.Flash_Init_err_cnt != 0U))
    {
        error_status |= APP_CONTROL_CORE_ERROR_FLASH_MASK;
    }
    if ((sys_ready.Baro_Ready != 1U) || (sys_err.Barometer_Init_err_cnt != 0U))
    {
        error_status |= APP_CONTROL_CORE_ERROR_BARO_MASK;
    }
    if ((sys_ready.IMU_Ready != 1U) || (sys_err.IMU_Init_err_cnt != 0U))
    {
        error_status |= APP_CONTROL_CORE_ERROR_IMU1_MASK;
    }
    if ((Sub_IMU_Flag != 1U) || (sys_err.IMU_Init_err_cnt != 0U))
    {
        error_status |= APP_CONTROL_CORE_ERROR_IMU2_MASK;
    }
    return error_status;
}

/**
* @brief 生成并发送主控错误状态到左右电机
* @param Timeout 单台电机回复超时时间
* @return LY_OK表示成功，负值表示失败
*/
int32_t APP_Control_SendCoreErrorStatus(uint32_t Timeout)
{
    APP_Control_CoreErrorStatus_u status;
    int32_t left_ret;
    int32_t right_ret;

    status.data.error_status = APP_Control_GetCoreErrorStatus();

    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = APP_CONTROL_CORE_ERROR_STATUS_CMD;
    control_buf[1] = status.buf[0];

    left_ret = app_control_send_command_timeout(Left, APP_CONTROL_CORE_ERROR_STATUS_LEN, Timeout);
    if (left_ret == LY_OK)
    {
        left_ret = fdcan_get_motor_receive_one_data((uint8_t)Left);
        if (left_ret != 0)
        {
            left_ret = (left_ret < 0) ? left_ret : -LY_ERR;
        }
    }

    right_ret = app_control_send_command_timeout(Right, APP_CONTROL_CORE_ERROR_STATUS_LEN, Timeout);
    if (right_ret == LY_OK)
    {
        right_ret = fdcan_get_motor_receive_one_data((uint8_t)Right);
        if (right_ret != 0)
        {
            right_ret = (right_ret < 0) ? right_ret : -LY_ERR;
        }
    }

    if ((left_ret != LY_OK) || (right_ret != LY_OK))
    {
        sys_err.Core_Error_Status_Send_err_cnt++;
        return (left_ret != LY_OK) ? left_ret : right_ret;
    }

    return LY_OK;
}

/**
* @brief 通知左右电机播放算法校准灯效
* @param Timeout 单台电机回复超时时间
* @return LY_OK表示左右电机均成功回复，负值表示失败
*/
int32_t APP_Control_StartAlgorithmCalibrationLight(uint32_t Timeout)
{
    int32_t left_ret;
    int32_t right_ret;

    memset(control_buf, 0, sizeof(control_buf));
    control_buf[0] = APP_CONTROL_ALGORITHM_CALIB_LIGHT_CMD;

    left_ret = app_control_send_command_timeout(Left, 1U, Timeout);
    right_ret = app_control_send_command_timeout(Right, 1U, Timeout);

    if (left_ret != LY_OK)
    {
        return left_ret;
    }
    if (right_ret != LY_OK)
    {
        return right_ret;
    }

    return LY_OK;
}
