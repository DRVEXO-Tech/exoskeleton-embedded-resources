/**
  ******************************************************************************
  * @file    Device_QMI8658A.h
  * @brief   QMI8658A 设备头文件。
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
#ifndef __DEVICE_QMI8658A_H__
#define __DEVICE_QMI8658A_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "PlatformSPI.h"
/* Private defines -----------------------------------------------------------*/
#define QMI8658_WHO_AM_I_REG                 0x00U /* 芯片标识寄存器，期望值见 QMI8658_WHO_AM_I。 */
#define QMI8658_REVISION_ID_REG              0x01U /* 芯片版本号。 */

#define QMI8658_CTRL1_REG                	 0x02U
#define QMI8658_CTRL1_SIM                    0x80U /* 1：三线SPI；0：四线SPI */
#define QMI8658_CTRL1_ADDR_AI                0x40U /* 1：地址自动递增 */
#define QMI8658_CTRL1_BE                     0x20U /* 1：大端；0：小端 */
#define QMI8658_CTRL1_INT2_EN                0x10U /* 1：使能INT2输出 */
#define QMI8658_CTRL1_INT1_EN                0x08U /* 1：使能INT1输出 */
#define QMI8658_CTRL1_FIFO_INT_SEL           0x04U /* 1：FIFO中断接INT1；0：INT2 */
#define QMI8658_CTRL1_SENSOR_DISABLE         0x01U /* 1：关闭内部高速振荡器 */

#define QMI8658_CTRL2_REG                    0x03U
#define QMI8658_CTRL2_AST                    0x80U /* 1：使能加速度自检 */
#define QMI8658_CTRL2_AFS_MASK               0x70U /* 加速度量程字段 */
#define QMI8658_CTRL2_AFS_2G                 0x00U /* ±2g */
#define QMI8658_CTRL2_AFS_4G                 0x10U /* ±4g */
#define QMI8658_CTRL2_AFS_8G                 0x20U /* ±8g */
#define QMI8658_CTRL2_AFS_16G                0x30U /* ±16g */
#define QMI8658_CTRL2_AODR_MASK              0x0FU
#define QMI8658_CTRL2_AODR_CODE_0            0x00U /* NA     / 7174.4 */
#define QMI8658_CTRL2_AODR_CODE_1            0x01U /* NA     / 3587.2 */
#define QMI8658_CTRL2_AODR_CODE_2            0x02U /* NA     / 1793.6 */
#define QMI8658_CTRL2_AODR_CODE_3            0x03U /* 1000   / 896.8 */
#define QMI8658_CTRL2_AODR_CODE_4            0x04U /* 500    / 448.4 */
#define QMI8658_CTRL2_AODR_CODE_5            0x05U /* 250    / 224.2 */
#define QMI8658_CTRL2_AODR_CODE_6            0x06U /* 125    / 112.1 */
#define QMI8658_CTRL2_AODR_CODE_7            0x07U /* 62.5   / 56.05 */
#define QMI8658_CTRL2_AODR_CODE_8            0x08U /* 31.25  / 28.025 */
#define QMI8658_CTRL2_AODR_LP_128HZ          0x0CU /* 占空比100% */
#define QMI8658_CTRL2_AODR_LP_21HZ           0x0DU /* 占空比58% */
#define QMI8658_CTRL2_AODR_LP_11HZ           0x0EU /* 占空比31% */
#define QMI8658_CTRL2_AODR_LP_3HZ            0x0FU /* 占空比8.5% */


#define QMI8658_CTRL3_REG                	 0x04U
#define QMI8658_CTRL3_GST                    0x80U /* 1：使能陀螺仪自检 */
#define QMI8658_CTRL3_GFS_MASK               0x70U /* 角速度量程字段 */
#define QMI8658_CTRL3_GFS_16DPS              0x00U /* ±16度/秒 */
#define QMI8658_CTRL3_GFS_32DPS              0x10U /* ±32度/秒 */
#define QMI8658_CTRL3_GFS_64DPS              0x20U /* ±64度/秒 */
#define QMI8658_CTRL3_GFS_128DPS             0x30U /* ±128度/秒 */
#define QMI8658_CTRL3_GFS_256DPS             0x40U /* ±256度/秒 */
#define QMI8658_CTRL3_GFS_512DPS             0x50U /* ±512度/秒 */
#define QMI8658_CTRL3_GFS_1024DPS            0x60U /* ±1024度/秒 */
#define QMI8658_CTRL3_GFS_2048DPS            0x70U /* ±2048度/秒 */
#define QMI8658_CTRL3_GODR_MASK              0x0FU
#define QMI8658_CTRL3_GODR_7174_4HZ          0x00U
#define QMI8658_CTRL3_GODR_3587_2HZ          0x01U
#define QMI8658_CTRL3_GODR_1793_6HZ          0x02U
#define QMI8658_CTRL3_GODR_896_8HZ           0x03U
#define QMI8658_CTRL3_GODR_448_4HZ           0x04U
#define QMI8658_CTRL3_GODR_224_2HZ           0x05U
#define QMI8658_CTRL3_GODR_112_1HZ           0x06U
#define QMI8658_CTRL3_GODR_56_05HZ           0x07U
#define QMI8658_CTRL3_GODR_28_025HZ          0x08U

#define QMI8658_CTRL5_REG                    0x06U
#define QMI8658_CTRL5_GLPF_EN                0x10U /* 1：使能陀螺仪低通滤波 */
#define QMI8658_CTRL5_GLPF_MODE_MASK         0x60U
#define QMI8658_CTRL5_GLPF_2_66_PERCENT      0x00U /* 带宽为ODR的2.66% */
#define QMI8658_CTRL5_GLPF_3_63_PERCENT      0x20U /* 带宽为ODR的3.63% */
#define QMI8658_CTRL5_GLPF_5_39_PERCENT      0x40U /* 带宽为ODR的5.39% */
#define QMI8658_CTRL5_GLPF_13_37_PERCENT     0x60U /* 带宽为ODR的13.37% */
#define QMI8658_CTRL5_ALPF_EN                0x01U /* 1：使能加速度低通滤波 */
#define QMI8658_CTRL5_ALPF_MODE_MASK         0x06U
#define QMI8658_CTRL5_ALPF_2_66_PERCENT      0x00U /* 带宽为ODR的2.66% */
#define QMI8658_CTRL5_ALPF_3_63_PERCENT      0x02U /* 带宽为ODR的3.63% */
#define QMI8658_CTRL5_ALPF_5_39_PERCENT      0x04U /* 带宽为ODR的5.39% */
#define QMI8658_CTRL5_ALPF_13_37_PERCENT     0x06U /* 带宽为ODR的13.37% */


#define QMI8658_CTRL7_REG                	 0x08U
#define QMI8658_CTRL7_SYNC_SAMPLE            0x80U /* 1：使能同步采样模式 */
#define QMI8658_CTRL7_DRDY_DIS               0x20U /* 1：禁止DRDY输出到INT2 */
#define QMI8658_CTRL7_GSN                    0x10U /* 1：陀螺仪Snooze模式，需GEN=1 */
#define QMI8658_CTRL7_GEN                    0x02U /* 1：使能陀螺仪 */
#define QMI8658_CTRL7_AEN                    0x01U /* 1：使能加速度计 */



#define QMI8658_CTRL8_REG                    0x09U
#define QMI8658_CTRL8_CTRL9_HANDSHAKE_TYPE   0x80U /* 1：STATUSINT.bit7握手；0：INT1 */
#define QMI8658_CTRL8_ACTIVITY_INT_SEL       0x40U /* 1：动作中断接INT1；0：INT2 */
#define QMI8658_CTRL8_SIG_MOTION_EN          0x08U /* 1：使能显著运动检测 */
#define QMI8658_CTRL8_NO_MOTION_EN           0x04U /* 1：使能静止检测 */
#define QMI8658_CTRL8_ANY_MOTION_EN          0x02U /* 1：使能任意运动检测 */
#define QMI8658_CTRL8_TAP_EN                 0x01U /* 1：使能敲击检测 */

#define QMI8658_CTRL9_REG          			 0x0AU /* 主机命令寄存器 */
#define QMI8658_CTRL9_COD_CMD      			 0xA2U /* COD校准命令 */
#define QMI8658_CTRL9_ACK_CMD      			 0x00U /* 命令应答 */

#define QMI8658_ACCEL_GYRO_DATA_REG    		 0x35U

#define QMI8658_RESET_REG          			 0x60U /* 软复位寄存器 */
#define QMI8658_RESET_CMD          			 0xB0U /* 软复位命令 */

#define QMI8658_RESET_DONE_REG     			 0x4DU /* 复位完成状态寄存器 */
#define QMI8658_RESET_DONE_VALUE   			 0x80U /* 复位完成标志 */

#define QMI8658_COD_STATUS_REG     			 0x46U /* COD校准结果 */

#define QMI8658_WHO_AM_I                 0x05U
#define QMI8658_REVISION_ID              0x7CU
/* Exported types ------------------------------------------------------------*/

typedef enum
{
    QMI8658_AFS_2G  = QMI8658_CTRL2_AFS_2G,
    QMI8658_AFS_4G  = QMI8658_CTRL2_AFS_4G,
    QMI8658_AFS_8G  = QMI8658_CTRL2_AFS_8G,
    QMI8658_AFS_16G = QMI8658_CTRL2_AFS_16G
} qmi8658_afs_en;

typedef enum
{
    QMI8658_GFS_16DPS   = QMI8658_CTRL3_GFS_16DPS,
    QMI8658_GFS_32DPS   = QMI8658_CTRL3_GFS_32DPS,
    QMI8658_GFS_64DPS   = QMI8658_CTRL3_GFS_64DPS,
    QMI8658_GFS_128DPS  = QMI8658_CTRL3_GFS_128DPS,
    QMI8658_GFS_256DPS  = QMI8658_CTRL3_GFS_256DPS,
    QMI8658_GFS_512DPS  = QMI8658_CTRL3_GFS_512DPS,
    QMI8658_GFS_1024DPS = QMI8658_CTRL3_GFS_1024DPS,
    QMI8658_GFS_2048DPS = QMI8658_CTRL3_GFS_2048DPS
} qmi8658_gfs_en;

typedef enum
{
    QMI8658_ODR_896_8HZ  = 0x03,
    QMI8658_ODR_448_4HZ  = 0x04,
    QMI8658_ODR_224_2HZ  = 0x05,
    QMI8658_ODR_112_1HZ  = 0x06,
    QMI8658_ODR_56_05HZ  = 0x07,
    QMI8658_ODR_28_025HZ = 0x08
} qmi8658_odr_en;

/* Exported extern variables ------------------------------------------------*/


/* Exported functions prototypes ---------------------------------------------*/

int32_t Device_QMI8658_ReadGeneral(struct platform_dev_spi_t *dev);

int32_t Device_QMI8658_Reset(struct platform_dev_spi_t *dev);

int32_t Device_QMI8658_Enable_ADDR_AI(struct platform_dev_spi_t *dev);

int32_t Device_QMI8658_COD_Start(struct platform_dev_spi_t *dev);

int32_t Device_QMI8658_COD_Acknowledge(struct platform_dev_spi_t *dev);

int32_t Device_QMI8658_COD_CheckResult(struct platform_dev_spi_t *dev);

int32_t Device_QMI8658_Enable(struct platform_dev_spi_t *dev);

int32_t Device_QMI8658_Disable(struct platform_dev_spi_t *dev);

int32_t Device_QMI8658_Set_AFS(struct platform_dev_spi_t *dev,qmi8658_afs_en range);

int32_t Device_QMI8658_Set_GFS(struct platform_dev_spi_t *dev, qmi8658_gfs_en range);

int32_t Device_QMI8658_SetODRSpeed(struct platform_dev_spi_t *dev, qmi8658_odr_en odr);

int32_t Device_QMI8658_ReadAccGyro(struct platform_dev_spi_t *dev,uint8_t *data,uint16_t lenth);

#ifdef __cplusplus
}
#endif
#endif   /* __DEVICE_QMI8658A_H__ */
