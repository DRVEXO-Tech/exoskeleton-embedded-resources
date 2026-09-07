/**
  ******************************************************************************
  * @file    APP_ISM6DSO.h
  * @brief   ISM6DSO的APP层头文件。
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
#ifndef __APP_ISM6DSO_H__
#define __APP_ISM6DSO_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Device_ISM6DSO.h"

/* Private defines -----------------------------------------------------------*/
#define ACC_Dynamic 0
#define AFS_2G
//这个数值是ISM6DSO官方数值 转到G(重力加速度)
#if(ACC_Dynamic == 0)
    #ifdef AFS_2G
        #define accSensitivity  0.000061035f        
    #endif
    #ifdef AFS_4G
        #define accSensitivity  0.000122070f
    #endif
    #ifdef AFS_8G
        #define accSensitivity  0.000244140f
    #endif
    #ifdef AFS_16G
        #define accSensitivity  0.000488281f
    #endif
    
#endif


#define GFS_2000DPS        
//这个数值是ISM6DSO官方数值 转换为dps（度每秒）
#ifdef GFS_125DPS    
    #define gyroSensitivity  0.004375f
#endif
#ifdef GFS_250DPS     
    #define gyroSensitivity  0.008750f
#endif
#ifdef GFS_500DPS    
    #define gyroSensitivity  0.017500f
#endif
#ifdef GFS_1000DPS    
    #define gyroSensitivity  0.035000f
#endif
#ifdef GFS_2000DPS    
    #define gyroSensitivity  0.070000f
#endif


/* Exported types ------------------------------------------------------------*/

/* Exported extern variables -------------------------------------------------*/
extern uint8_t Sub_IMU_Flag;

extern float ACC_g[3];//单位重力加速度g
extern float GRYO_dps[3];//单位度每秒
extern float Temperature;
extern float dTs;

extern float ACC_g_sub[3];//单位重力加速度g
extern float GRYO_dps_sub[3];//单位度每秒
extern float dTs_sub;
/* Exported functions prototypes ---------------------------------------------*/
int16_t ISM6DSO_Init(void);
void IMU_Data_Task(uint16_t dT_ms);
void IMU_Data_Sub_Task(uint16_t dT_ms);

#ifdef __cplusplus
}
#endif
#endif   /* __APP_ISM6DSO_H__ */
