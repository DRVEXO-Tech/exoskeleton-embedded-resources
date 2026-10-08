/**
  ******************************************************************************
  * @file           : Device_QMI8658A.c
  * @brief          : QMI8658A 设备文件
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
#include "Device_QMI8658A.h"

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/


/* Function ------------------------------------------------------------------*/


int32_t Device_QMI8658_Enable(struct platform_dev_spi_t *dev)
{
	int32_t ret;
    uint8_t value;
	ret = dev->read(dev, QMI8658_CTRL7_REG, &value, 1);
    if (ret != RET_OK)
        return ret;
	value |= QMI8658_CTRL7_AEN | QMI8658_CTRL7_GEN;
	ret = dev->write(dev, QMI8658_CTRL7_REG, &value, 1U);
    if (ret != RET_OK)
        return ret;
    HAL_Delay(2);
	return RET_OK;
	
}



int32_t Device_QMI8658_Disable(struct platform_dev_spi_t *dev)
{
	int32_t ret;
    uint8_t value;
	ret = dev->read(dev, QMI8658_CTRL7_REG, &value, 1);
    if (ret != RET_OK)
        return ret;
	value &= (uint8_t)~(QMI8658_CTRL7_AEN | QMI8658_CTRL7_GEN);
	 ret = dev->write(dev, QMI8658_CTRL7_REG, &value, 1U);
    if (ret != RET_OK)
        return ret;
    HAL_Delay(2);
	return RET_OK;
}


int32_t Device_QMI8658_ReadAccGyro(struct platform_dev_spi_t *dev,uint8_t *data,uint16_t lenth)
{
	int32_t ret;
	ret = dev->read(dev,QMI8658_ACCEL_GYRO_DATA_REG,data,lenth);
    if (ret != RET_OK)
        return ret;
	return RET_OK;
}

//设置加速度量程
int32_t Device_QMI8658_Set_AFS(struct platform_dev_spi_t *dev,qmi8658_afs_en range)
{
	int32_t ret;
    uint8_t value;
    uint8_t range_value =range;
    ret = dev->read(dev, QMI8658_CTRL2_REG, &value, 1);
    if (ret != RET_OK)
        return ret;
    value = (uint8_t)((value & (uint8_t)~QMI8658_CTRL2_AFS_MASK) |range_value);
    return dev->write(dev, QMI8658_CTRL2_REG, &value, 1);
}

//设置角速度量程
int32_t Device_QMI8658_Set_GFS(struct platform_dev_spi_t *dev, qmi8658_gfs_en range)
{
	int32_t ret;
    uint8_t value;
    uint8_t range_value = range;
    ret = dev->read(dev, QMI8658_CTRL3_REG, &value, 1);
    if (ret != RET_OK)
        return ret;
    value = (uint8_t)((value & (uint8_t)~QMI8658_CTRL3_GFS_MASK) |range_value);
    return dev->write(dev, QMI8658_CTRL3_REG, &value, 1);
}

//设置输出频率
int32_t Device_QMI8658_SetODRSpeed(struct platform_dev_spi_t *dev, qmi8658_odr_en odr)
{
	int32_t ret;
    uint8_t ctrl2;
    uint8_t ctrl3;
    uint8_t odr_value = odr;
	ret = dev->read(dev, QMI8658_CTRL2_REG, &ctrl2, 1);
    if (ret != RET_OK)
        return ret;
    ret = dev->read(dev, QMI8658_CTRL3_REG, &ctrl3, 1);
    if (ret != RET_OK)
        return ret;
    ctrl2 = (uint8_t)((ctrl2 & (uint8_t)~QMI8658_CTRL2_AODR_MASK) |odr_value);
    ctrl3 = (uint8_t)((ctrl3 & (uint8_t)~QMI8658_CTRL3_GODR_MASK) |odr_value);
    ret = dev->write(dev, QMI8658_CTRL2_REG, &ctrl2, 1);
    if (ret != RET_OK)
        return ret;
    return dev->write(dev, QMI8658_CTRL3_REG, &ctrl3, 1);
}



//打开地址自增
int32_t Device_QMI8658_Enable_ADDR_AI(struct platform_dev_spi_t *dev)
{
	int32_t ret; 
	uint8_t ctrl_1;
	ret = dev->read(dev,QMI8658_CTRL1_REG,&ctrl_1,1);
	if (ret != RET_OK)
		return ret;
	ctrl_1 |= QMI8658_CTRL1_ADDR_AI ;
	ret = dev->write(dev,QMI8658_CTRL1_REG,&ctrl_1,1);
	if (ret != RET_OK)
		return ret;
	return RET_OK;
}

//复位
int32_t Device_QMI8658_Reset(struct platform_dev_spi_t *dev)
{
	int32_t ret;
	uint8_t value = QMI8658_RESET_CMD;
	ret = dev->write(dev, QMI8658_RESET_REG, &value, 1U);
	HAL_Delay(10);
	for (int i = 0; i < 500; i++)
    {
        HAL_Delay(1U);
        ret = dev->read(dev, QMI8658_RESET_DONE_REG, &value, 1U);
        if (ret != RET_OK)
            return ret;
        if (value == QMI8658_RESET_DONE_VALUE)
            return RET_OK;
    }
    return -RET_TIMEOUT;
}


int32_t Device_QMI8658_COD_Start(struct platform_dev_spi_t *dev)
{
	int32_t ret;
	uint8_t value = QMI8658_CTRL9_COD_CMD;
	return dev->write(dev, QMI8658_CTRL9_REG, &value, 1);
}

int32_t Device_QMI8658_COD_Acknowledge(struct platform_dev_spi_t *dev)
{
	int32_t ret;
	uint8_t value = QMI8658_CTRL9_ACK_CMD;
	
	return dev->write(dev, QMI8658_CTRL9_REG, &value, 1);
}

int32_t Device_QMI8658_COD_CheckResult(struct platform_dev_spi_t *dev)
{
	int32_t ret;
	uint8_t cod_status = 0xFF;
	ret = dev->read(dev, QMI8658_COD_STATUS_REG, &cod_status, 1);
    if (ret != RET_OK)
        return ret;
    if (cod_status != 0)
        return -RET_ERROR;
	return RET_OK;
}


//读取芯片ID
int32_t Device_QMI8658_ReadGeneral(struct platform_dev_spi_t *dev)
{
    int32_t ret;
    uint8_t Whoami;
	uint8_t Revision_ID;

    ret = dev->read(dev,QMI8658_WHO_AM_I_REG,&Whoami, 1);
    if (ret != RET_OK)
		return ret;
    if (Whoami != QMI8658_WHO_AM_I)
		return RET_ERROR;
	ret = dev->read(dev,QMI8658_REVISION_ID_REG,&Revision_ID, 1);
    if (ret != RET_OK)
		return ret;
    if (Revision_ID != QMI8658_REVISION_ID)
		return RET_ERROR;
    return RET_OK;
}


