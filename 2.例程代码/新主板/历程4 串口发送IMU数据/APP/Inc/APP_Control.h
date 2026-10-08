/**
  ******************************************************************************
  * @file    APP_Control.h
  * @brief   控制电机的APP层头文件。
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
#ifndef __APP_CONTROL_H__
#define __APP_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    Left = 0,
    Right = 1,
} Direction_en;

typedef enum
{
    CONTROL_MODE_TORQUE_RAMP = 0,
    CONTROL_MODE_VELOCITY_RAMP = 1,
    CONTROL_MODE_POSITION_FILTER = 2,
    CONTROL_MODE_POSITION_PROFILE = 3,
    CONTROL_MODE_MIT = 4,
} Mode_en;

/* Exported functions prototypes ---------------------------------------------*/
int32_t APP_Control_SetMode(Direction_en direction, Mode_en mode,uint32_t Timeout);
int32_t APP_Control_Enable(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_Disable(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_SetTorque(Direction_en direction, float torque);
int32_t APP_Control_SetVelocity(Direction_en direction, float velocity);
int32_t APP_Control_SetPosition(Direction_en direction, float position);
int32_t APP_Control_Sync(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_CalibStart(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_CalibReport(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_CalibAbort(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_AnticoggingStart(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_AnticoggingReport(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_AnticoggingAbort(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_SetHome(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_ErrorReset(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_GetStatusword(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_StatuswordReport(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_GetValue1(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_GetValue2(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_Heartbeat(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_SetConfig(Direction_en direction, int32_t index, uint32_t value, uint32_t Timeout);
int32_t APP_Control_GetConfig(Direction_en direction, int32_t index, uint32_t value, uint32_t Timeout);
int32_t APP_Control_SaveAllConfig(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_ResetAllConfig(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_GetFwVersion(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_SetMITMode(Direction_en direction, float position, float velocity, float kp, float kd, float torque);
int32_t APP_Control_SetStdId(Direction_en direction, uint16_t id, uint32_t Timeout);

int32_t APP_Control_Sleep(uint32_t Timeout);
int32_t APP_Control_Set_Brightness(uint8_t Brightness,uint32_t Timeout);
int32_t APP_Control_TestFan(uint8_t state, uint32_t Timeout);
int32_t APP_Control_TestKeyRGB(uint8_t state, uint32_t Timeout);
int32_t APP_Control_ReadCalibrateState(uint8_t *state, uint32_t Timeout);
int32_t APP_Control_DisableDisconnectionAlarm(Direction_en direction, uint32_t Timeout);
int32_t APP_Control_CalibStart2(Direction_en direction, uint32_t Timeout);
uint8_t APP_Control_GetCoreErrorStatus(void);
int32_t APP_Control_SendCoreErrorStatus(uint32_t Timeout);
int32_t APP_Control_StartAlgorithmCalibrationLight(uint32_t Timeout);
#ifdef __cplusplus
}
#endif
#endif   /* __APP_CONTROL_H__ */
