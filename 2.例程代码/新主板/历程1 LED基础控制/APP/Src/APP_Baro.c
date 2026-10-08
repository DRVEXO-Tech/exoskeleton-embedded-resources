/**
  ******************************************************************************
  * @file           : APP_Baro.c
  * @brief          : 气压计应用层文件
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
#include "APP_Baro.h"

/* Define ------------------------------------------------------------------*/
#define APP_BARO_KALMAN_PROCESS_NOISE       0.01f
#define APP_BARO_KALMAN_MEASUREMENT_NOISE   0.25f

/* Variable ------------------------------------------------------------------*/
struct Baro_Data_t Baro_Data;
/* Function ------------------------------------------------------------------*/

/**
  * @brief  读取BMP580数据并进行气压卡尔曼滤波。
  * @param  dT_ms 任务周期，单位毫秒。
  * @return 无。
  */
void APP_Baro_Task(uint16_t dT_ms)
{
    static uint8_t zero_ready = 0;
    static float zero_altitude = 0.0f;
    static uint8_t kalman_ready = 0U;
    static float kalman_pressure_hPa = 0.0f;
    static float kalman_covariance = 1.0f;
    uint8_t ready = 0;
    uint8_t buf[6];
    int32_t temperature_raw;
    uint32_t pressure_raw;
    float pressure_hPa;
    float kalman_gain;

    if (dT_ms == 0)
        return;

    if (Device_BMP580_Data_Ready(&ready) != RET_OK)
    {
        sys_err.Barometer_Read_err_cnt++;
        return;
    }
    if (ready == 0)
        return;

    if (Device_BMP580_Read(buf) != RET_OK)
    {
        sys_err.Barometer_Read_err_cnt++;
        return;
    }

    /* 温度为低字节在前的24位有符号数。 */
    temperature_raw = (int32_t)(((uint32_t)buf[2] << 16) |
                               ((uint32_t)buf[1] << 8) | buf[0]);
    if (temperature_raw & 0x800000)
        temperature_raw -= 0x1000000;

    /* 压力为低字节在前的24位无符号数。 */
    pressure_raw = ((uint32_t)buf[5] << 16) |
                   ((uint32_t)buf[4] << 8) | buf[3];
    if (pressure_raw == 0)
    {
        sys_err.Barometer_Read_err_cnt++;
        return;
    }

    Baro_Data.temperature = temperature_raw / 65536.0f;
    pressure_hPa = pressure_raw / 6400.0f;

    /* 对压力值进行一维卡尔曼滤波，抑制测量噪声后再计算海拔。 */
    if (kalman_ready == 0U)
    {
        kalman_pressure_hPa = pressure_hPa;
        kalman_ready = 1U;
    }
    else
    {
        kalman_covariance += APP_BARO_KALMAN_PROCESS_NOISE;
        kalman_gain = kalman_covariance /
                      (kalman_covariance + APP_BARO_KALMAN_MEASUREMENT_NOISE);
        kalman_pressure_hPa += kalman_gain * (pressure_hPa - kalman_pressure_hPa);
        kalman_covariance *= 1.0f - kalman_gain;
    }
    Baro_Data.pressure_hPa = kalman_pressure_hPa;

    /* 按标准海平面气压1013.25hPa估算海拔，单位m。 */
    Baro_Data.Absolute_elevation = 44330.0f *
        (1.0f - powf(Baro_Data.pressure_hPa / 1013.25f, 0.190295f));

    if (zero_ready == 0)
    {
        zero_altitude = Baro_Data.Absolute_elevation;
        zero_ready = 1;
    }
    Baro_Data.Relative_elevation =
        Baro_Data.Absolute_elevation - zero_altitude;
	Baro_Data.sample_sequence++;
}

