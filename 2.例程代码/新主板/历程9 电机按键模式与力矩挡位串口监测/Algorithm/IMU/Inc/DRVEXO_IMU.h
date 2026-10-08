/**
  ******************************************************************************
  * @file    DRVEXO_IMU.h
  * @brief   IMU 数据与姿态解算头文件。
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
#ifndef __DRVEXO_IMU_H__
#define __DRVEXO_IMU_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DRVEXOIC.h"
/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* 机体坐标沿用前方为+x、左方为+y；保留旧类型名供现有调用处使用。 */
typedef struct imu_data_t {
    float IMU_ACC_g[3];     /* 加速度，单位g，包含重力。 */
    float IMU_GRYO_dps[3];  /* 角速度，单位度/秒。 */
    float Temperature;     /* 兼容字段，当前QMI8658采样未读取温度，固定为0。 */
    float dTs;             /* 两次参与解算的采样时间间隔，单位秒。 */
} IMU_Data_st;

typedef enum {
    IMU1 = 0,
    IMU2,
    IMU_Num,
} IMU_Num_en;

/* 快速零偏校准调试量，校准完成后保持最后一次值。 */
struct imu_gyro_calib_debug_t {
    float delta_dps;       /* 原始角速度与低通值之差的模长，单位度/秒。 */
    float hold_ms;         /* 连续稳定时间，单位ms。 */
    uint16_t sample_count; /* 当前累计校准样本数。 */
};
/* Exported extern variables -------------------------------------------------*/
extern struct imu_gyro_calib_debug_t imu_gyro_calib_debug[IMU_Num];
extern uint8_t fast_gyro_calib_done[IMU_Num];
extern IMU_Data_st IMU_Data[IMU_Num];
extern IMU_Data_st IMU_Data_IEM[IMU_Num];
extern DRVEXOAhrs ahrs;
extern DRVEXOEuler euler;
extern float imu_dbg_earth_acc_bias_g[3];
extern uint8_t imu_earth_acc_bias_ready;
/* Exported functions prototypes ---------------------------------------------*/
void IMU_Data_Init(void);
void IMU_Update(IMU_Data_st *IMU_Data);
void IMU_Updata_Task(uint16_t dT);
#ifdef __cplusplus
}
#endif
#endif   /* __DRVEXO_IMU_H__ */
