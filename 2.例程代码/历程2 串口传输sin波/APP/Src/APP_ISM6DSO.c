/**
  ******************************************************************************
  * @file           : APP_ISM6DSO.c
  * @brief          : ISM6DSO APP层文件
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
#include "APP_ISM6DSO.h"

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/
//从IMU是否存在标志位
uint8_t Sub_IMU_Flag=0;
//下面变量是和时间戳相关
static uint32_t g_imu1_prev_timestamp;
static uint32_t g_imu2_prev_timestamp;
static uint8_t g_imu1_first_run = 1;
static uint8_t g_imu2_first_run = 1;


//
float ACC_g[3];//单位重力加速度g
float GRYO_dps[3];//单位度每秒
float Temperature;
float dTs;

float ACC_g_sub[3];//单位重力加速度g
float GRYO_dps_sub[3];//单位度每秒
float dTs_sub;

/* Function ------------------------------------------------------------------*/
/**
* @brief 复位IMU运行时状态
* @param 无
* @return 无
*/
static void app_ism6dso_reset_runtime_state(void)
{
    g_imu1_prev_timestamp = 0;
    g_imu2_prev_timestamp = 0;
    g_imu1_first_run = 1;
    g_imu2_first_run = 1;
}

/**
* @brief 初始化主副IMU
* @param 无
* @return 初始化结果
*/
int16_t ISM6DSO_Init(void)
{
    static uint8_t whoamI, rst;

    app_ism6dso_reset_runtime_state();

    lsm6dso_device_id_get(&platform_spi_ism6dso1_dev, &whoamI);
    if (whoamI != LSM6DSO_ID)
        return -1;
    lsm6dso_reset_set(&platform_spi_ism6dso1_dev, PROPERTY_ENABLE);
    do {
        lsm6dso_reset_get(&platform_spi_ism6dso1_dev, &rst);
    } while (rst);
    lsm6dso_i3c_disable_set(&platform_spi_ism6dso1_dev, LSM6DSO_I3C_DISABLE);
    lsm6dso_block_data_update_set(&platform_spi_ism6dso1_dev, PROPERTY_ENABLE);
    lsm6dso_xl_data_rate_set(&platform_spi_ism6dso1_dev, LSM6DSO_XL_ODR_104Hz);
    lsm6dso_gy_data_rate_set(&platform_spi_ism6dso1_dev, LSM6DSO_GY_ODR_104Hz);
    lsm6dso_xl_full_scale_set(&platform_spi_ism6dso1_dev, LSM6DSO_2g);
    lsm6dso_gy_full_scale_set(&platform_spi_ism6dso1_dev, LSM6DSO_2000dps);
    lsm6dso_xl_hp_path_on_out_set(&platform_spi_ism6dso1_dev, LSM6DSO_LP_ODR_DIV_100);
    lsm6dso_xl_filter_lp2_set(&platform_spi_ism6dso1_dev, PROPERTY_ENABLE);
    lsm6dso_timestamp_set(&platform_spi_ism6dso1_dev, PROPERTY_ENABLE);
    
    static uint8_t whoamI2=0, rst_2;


    lsm6dso_device_id_get(&platform_spi_ism6dso2_dev, &whoamI2);

    if (whoamI2 != LSM6DSO_ID)
        return 0;
    lsm6dso_reset_set(&platform_spi_ism6dso2_dev, PROPERTY_ENABLE);
    do {
        lsm6dso_reset_get(&platform_spi_ism6dso2_dev, &rst_2);
    } while (rst_2);
    lsm6dso_i3c_disable_set(&platform_spi_ism6dso2_dev, LSM6DSO_I3C_DISABLE);
    lsm6dso_block_data_update_set(&platform_spi_ism6dso2_dev, PROPERTY_ENABLE);
    lsm6dso_xl_data_rate_set(&platform_spi_ism6dso2_dev, LSM6DSO_XL_ODR_104Hz);
    lsm6dso_gy_data_rate_set(&platform_spi_ism6dso2_dev, LSM6DSO_GY_ODR_104Hz);
    lsm6dso_xl_full_scale_set(&platform_spi_ism6dso2_dev, LSM6DSO_2g);
    lsm6dso_gy_full_scale_set(&platform_spi_ism6dso2_dev, LSM6DSO_2000dps);
    lsm6dso_xl_hp_path_on_out_set(&platform_spi_ism6dso2_dev, LSM6DSO_LP_ODR_DIV_100);
    lsm6dso_xl_filter_lp2_set(&platform_spi_ism6dso2_dev, PROPERTY_ENABLE);
    lsm6dso_timestamp_set(&platform_spi_ism6dso2_dev, PROPERTY_ENABLE);
    return 1;
}

/**
* @brief 主IMU数据采集任务
* @param dT_ms 任务周期
* @return 无
*/
void IMU_Data_Task(uint16_t dT_ms)
{
    uint8_t reg;
    int16_t data_raw_acceleration[3]; 
    int16_t data_raw_angular_rate[3]; 
    int16_t data_raw_temperature;

    lsm6dso_xl_flag_data_ready_get(&platform_spi_ism6dso1_dev, &reg);
    if (reg) {
      memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
      lsm6dso_acceleration_raw_get(&platform_spi_ism6dso1_dev, data_raw_acceleration);
      ACC_g[0] =accSensitivity*data_raw_acceleration[0];
      ACC_g[1] =accSensitivity*data_raw_acceleration[1];
      ACC_g[2] =accSensitivity*data_raw_acceleration[2];

    }

    lsm6dso_gy_flag_data_ready_get(&platform_spi_ism6dso1_dev, &reg);
    if (reg) {
      memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
      lsm6dso_angular_rate_raw_get(&platform_spi_ism6dso1_dev, data_raw_angular_rate);
      GRYO_dps[0] =data_raw_angular_rate[0]*gyroSensitivity;
      GRYO_dps[1] =data_raw_angular_rate[1]*gyroSensitivity;
      GRYO_dps[2] =data_raw_angular_rate[2]*gyroSensitivity;
    }

    lsm6dso_temp_flag_data_ready_get(&platform_spi_ism6dso1_dev, &reg);
    if (reg) {
      memset(&data_raw_temperature, 0x00, sizeof(int16_t));
      lsm6dso_temperature_raw_get(&platform_spi_ism6dso1_dev, &data_raw_temperature);
      Temperature =lsm6dso_from_lsb_to_celsius(data_raw_temperature);
    }

    uint32_t timestamp_raw = 0;
    lsm6dso_timestamp_raw_get(&platform_spi_ism6dso1_dev, &timestamp_raw);
    if (g_imu1_first_run) {
        g_imu1_prev_timestamp = timestamp_raw;
        g_imu1_first_run = 0;
         dTs = 0.002f; 
    } else {
        uint32_t tick_diff = timestamp_raw - g_imu1_prev_timestamp;
        g_imu1_prev_timestamp = timestamp_raw;
         dTs = (float)tick_diff * 0.000025f; 
    }


}

/**
* @brief 副IMU数据采集任务
* @param dT_ms 任务周期
* @return 无
*/
void IMU_Data_Sub_Task(uint16_t dT_ms)
{
    uint8_t reg;
    int16_t data_raw_acceleration[3]; 
    int16_t data_raw_angular_rate[3]; 
    int16_t data_raw_temperature;
    
    lsm6dso_xl_flag_data_ready_get(&platform_spi_ism6dso2_dev, &reg);
    if (reg) {
      memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
      lsm6dso_acceleration_raw_get(&platform_spi_ism6dso2_dev, data_raw_acceleration);
      ACC_g_sub[0] =accSensitivity*data_raw_acceleration[0];
      ACC_g_sub[1] =accSensitivity*data_raw_acceleration[1];
      ACC_g_sub[2] =accSensitivity*data_raw_acceleration[2];
    }

    lsm6dso_gy_flag_data_ready_get(&platform_spi_ism6dso2_dev, &reg);
    if (reg) {
      memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
      lsm6dso_angular_rate_raw_get(&platform_spi_ism6dso2_dev, data_raw_angular_rate);
      GRYO_dps_sub[0] =data_raw_angular_rate[0]*gyroSensitivity;
      GRYO_dps_sub[1] =data_raw_angular_rate[1]*gyroSensitivity;
      GRYO_dps_sub[2] =data_raw_angular_rate[2]*gyroSensitivity;

    }

     uint32_t timestamp_raw = 0;
     lsm6dso_timestamp_raw_get(&platform_spi_ism6dso2_dev, &timestamp_raw);
    if (g_imu2_first_run) {
        g_imu2_prev_timestamp = timestamp_raw;
        g_imu2_first_run = 0;
        dTs_sub = 0.002f; 
    } else {
        uint32_t tick_diff = timestamp_raw - g_imu2_prev_timestamp;
        g_imu2_prev_timestamp = timestamp_raw;
        dTs_sub = (float)tick_diff * 0.000025f; 
    }


}
