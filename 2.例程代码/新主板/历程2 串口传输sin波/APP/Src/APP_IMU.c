/**
  ******************************************************************************
  * @file           : APP_IMU.c
  * @brief          : IMU 应用文件
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
#include "APP_IMU.h"

/* Define ------------------------------------------------------------------*/


/* Variable ------------------------------------------------------------------*/
float acc_g[3];
float gyro_dps[3];
float acc_g_sub[3];
float gyro_dps_sub[3];

uint8_t Sub_IMU_Flag=0;
/* Function ------------------------------------------------------------------*/

static int32_t APP_IMU_PrepareDevice(struct platform_dev_spi_t *dev)
{
    int32_t ret;

    ret = Device_QMI8658_ReadGeneral(dev);
    if (ret != RET_OK)
        return ret;

    ret = Device_QMI8658_Reset(dev);
    if (ret != RET_OK)
        return ret;

    ret = Device_QMI8658_Enable_ADDR_AI(dev);
    if (ret != RET_OK)
        return ret;

    ret = Device_QMI8658_Disable(dev);
    if (ret != RET_OK)
        return ret;

    ret = Device_QMI8658_Set_AFS(dev, QMI8658_AFS_8G);
    if (ret != RET_OK)
        return ret;

    ret = Device_QMI8658_Set_GFS(dev, QMI8658_GFS_1024DPS);
    if (ret != RET_OK)
        return ret;

    ret = Device_QMI8658_SetODRSpeed(dev, QMI8658_ODR_112_1HZ);
    if (ret != RET_OK)
        return ret;

    return RET_OK;
}
int32_t APP_IMU_Init(void)
{
    int32_t ret;

    /* 主QMI8658必须初始化成功。 */
    ret = APP_IMU_PrepareDevice(&platform_dev_spi_qmi8658);
    if (ret != RET_OK)
        return ret;

    /* 副QMI8658初始化失败时，仅使用主QMI8658。 */
    ret = APP_IMU_PrepareDevice(&platform_dev_spi_qmi8658_sub);
    if (ret == RET_OK)
        Sub_IMU_Flag = 1;

    /* 同时启动主、副陀螺仪COD校准。 */
    ret = Device_QMI8658_COD_Start(&platform_dev_spi_qmi8658);
    if (ret != RET_OK)
        return ret;

    if (Sub_IMU_Flag != 0)
    {
        ret = Device_QMI8658_COD_Start(&platform_dev_spi_qmi8658_sub);
        if (ret != RET_OK)
            Sub_IMU_Flag = 0;
    }

    HAL_Delay(1500);

    ret = Device_QMI8658_COD_Acknowledge(&platform_dev_spi_qmi8658);
    if (ret != RET_OK)
        return ret;

    HAL_Delay(10);

    ret = Device_QMI8658_COD_CheckResult(&platform_dev_spi_qmi8658);
    if (ret != RET_OK)
        return ret;

    if (Sub_IMU_Flag != 0)
    {
        ret = Device_QMI8658_COD_Acknowledge(&platform_dev_spi_qmi8658_sub);
        if (ret != RET_OK)
        {
            Sub_IMU_Flag = 0;
        }
        else
        {
            HAL_Delay(10);
            ret = Device_QMI8658_COD_CheckResult(&platform_dev_spi_qmi8658_sub);
            if (ret != RET_OK)
                Sub_IMU_Flag = 0;
        }
    }

    /* 开启主陀螺仪六轴采样。 */
    ret = Device_QMI8658_Enable(&platform_dev_spi_qmi8658);
    if (ret != RET_OK)
        return ret;

    /* 副陀螺仪可用时开启副陀螺仪六轴采样。 */
    if (Sub_IMU_Flag != 0)
    {
        ret = Device_QMI8658_Enable(&platform_dev_spi_qmi8658_sub);
        if (ret != RET_OK)
            Sub_IMU_Flag = 0;
    }

    HAL_Delay(300);

    return RET_OK;
}

void APP_IMU_Data(uint16_t dT_ms)
{
	int32_t ret;
	uint8_t buf[12];
	int16_t acc_raw[3],gyro_raw[3];
	Device_QMI8658_ReadAccGyro(&platform_dev_spi_qmi8658,buf,sizeof(buf));
	acc_raw[0] = (int16_t)(((uint16_t)buf[1] << 8U) | buf[0]);
    acc_raw[1] = (int16_t)(((uint16_t)buf[3] << 8U) | buf[2]);
    acc_raw[2] = (int16_t)(((uint16_t)buf[5] << 8U) | buf[4]);
    gyro_raw[0] = (int16_t)(((uint16_t)buf[7] << 8U) | buf[6]);
    gyro_raw[1] = (int16_t)(((uint16_t)buf[9] << 8U) | buf[8]);
    gyro_raw[2] = (int16_t)(((uint16_t)buf[11] << 8U) | buf[10]);
	for (uint8_t i = 0U; i < 3U; i++)
	{
		acc_g[i] = (float)acc_raw[i] / 4096.0f;
		gyro_dps[i] = (float)gyro_raw[i] / 32.0f;
	}
	if(Sub_IMU_Flag)
	{
		Device_QMI8658_ReadAccGyro(&platform_dev_spi_qmi8658_sub,buf,sizeof(buf));
		acc_raw[0] = (int16_t)(((uint16_t)buf[1] << 8U) | buf[0]);
		acc_raw[1] = (int16_t)(((uint16_t)buf[3] << 8U) | buf[2]);
		acc_raw[2] = (int16_t)(((uint16_t)buf[5] << 8U) | buf[4]);
		gyro_raw[0] = (int16_t)(((uint16_t)buf[7] << 8U) | buf[6]);
		gyro_raw[1] = (int16_t)(((uint16_t)buf[9] << 8U) | buf[8]);
		gyro_raw[2] = (int16_t)(((uint16_t)buf[11] << 8U) | buf[10]);
		for (uint8_t i = 0U; i < 3U; i++)
		{
			acc_g_sub[i] = (float)acc_raw[i] / 4096.0f;
			gyro_dps_sub[i] = (float)gyro_raw[i] / 32.0f;
		}
	}
	
}

