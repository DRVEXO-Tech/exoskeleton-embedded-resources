/**
  ******************************************************************************
  * @file           : Device_ZB35Q01xx.c
  * @brief          : ZB35Q01xx驱动层文件
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
#include "Device_ZB35Q01xx.h"

/* Define ------------------------------------------------------------------*/
#define Delay_ms(x) HAL_Delay(x)
/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/
int32_t Device_ZB35Q01_ReadGeneral(void)
{
	uint8_t cmd[2] = {ZB35Q01XX_CMD_READ_ID, 0x00};
	uint8_t id[2] = {0};
	int32_t ret;
	ret = platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01,cmd,sizeof(cmd),id,sizeof(id));
	if(ret<0)
		return ret;
	if(id[0] != ZB35Q01XX_MFR_ID || id[1] != ZB35Q01XX_DEVICE_ID)
		return -RET_ERROR;
	return RET_OK;
}
int32_t Device_ZB35Q01_EraseBlock(uint16_t block)
{
	uint32_t page = (uint32_t)block * 64U;
    uint8_t cmd[4];
    uint8_t status_cmd[2] = {0x0F, 0xC0};
    uint8_t status = 0;
	//写使能
	cmd[0] = ZB35Q01XX_CMD_WRITE_ENABLE;
    platform_dev_spi_zb35q01.write_cmd(&platform_dev_spi_zb35q01, cmd, 1, NULL, 0);
    platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01, status_cmd, 2, &status, 1);
    if (!(status & 0x02))
        return -RET_NOT_READY;
	cmd[0] = 0xD8;
    cmd[1] = page >> 16;
    cmd[2] = page >> 8;
    cmd[3] = page;
    platform_dev_spi_zb35q01.write_cmd(&platform_dev_spi_zb35q01, cmd, 4, NULL, 0);
    //等待擦除结束检查E_FAIL
    for (uint8_t i = 0; i < 100; i++)
    {
        platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01, status_cmd, 2, &status, 1);
        if (!(status & 0x01))
            return (status & 0x04) ? -RET_ERROR : RET_OK;
        Delay_ms(1);
    }
    return -RET_TIMEOUT;
}


int32_t Device_ZB35Q01_ReadPage(uint32_t page, uint16_t offset,uint8_t *data, uint16_t len)
{
	uint8_t cmd[4];
    uint8_t status_cmd[] = {0x0F, 0xC0};
    uint8_t status = 0x01;
    uint8_t i;
    uint16_t size;
	//将指定页读取到芯片缓存
    cmd[0] = 0x13;
    cmd[1] = page >> 16;
    cmd[2] = page >> 8;
    cmd[3] = page;
    platform_dev_spi_zb35q01.write_cmd(&platform_dev_spi_zb35q01, cmd, 4, NULL, 0);
	 /* 等待读取完成。 */
    for (i = 0; i < 100; i++)
    {
        platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01, status_cmd, 2, &status, 1);
        if (!(status & 0x01))
            break;

        Delay_ms(1);
    }
    if (i >= 100)
        return -RET_TIMEOUT;
    /* 内置ECC开启时，10表示存在不可纠正的错误。 */
    if ((status & 0x30) == 0x20)
        return -RET_ERROR;

    /* 按平台缓存大小分段读取，每段重新指定页内偏移。 */
   /* 从缓存指定偏移开始，一次读取len字节。 */
    cmd[0] = 0x03;
    cmd[1] = offset >> 8;
    cmd[2] = offset;
    cmd[3] = 0x00;
    return platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01, cmd, 4, data, len);
}
int32_t Device_ZB35Q01_WritePage(uint32_t page, uint16_t offset,const uint8_t *data, uint16_t len)
{
	uint8_t cmd[4];
	uint8_t status_cmd[] = {0x0F, 0xC0};
	uint8_t status = 0;
	cmd[0] = ZB35Q01XX_CMD_PLOAD;
	cmd[1] = offset >> 8;
    cmd[2] = offset;
	//载入缓存地址
	platform_dev_spi_zb35q01.write_cmd(&platform_dev_spi_zb35q01, cmd, 3, data, len);
	//写使能
	cmd[0] = ZB35Q01XX_CMD_WRITE_ENABLE;
    platform_dev_spi_zb35q01.write_cmd(&platform_dev_spi_zb35q01, cmd, 1, NULL, 0);
	//判断是否写使能
	platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01, status_cmd, 2, &status, 1);
    if (!(status & 0x02))
        return -RET_NOT_READY;
	//缓存写入指定页
    cmd[0] = 0x10;
    cmd[1] = page >> 16;
    cmd[2] = page >> 8;
    cmd[3] = page;
    platform_dev_spi_zb35q01.write_cmd(&platform_dev_spi_zb35q01, cmd, 4, NULL, 0);
    //等待写完
    for (uint8_t i = 0; i < 100; i++)
    {
        platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01, status_cmd, 2, &status, 1);
        if (!(status & 0x01))
            return (status & 0x08) ? -RET_ERROR : RET_OK;
        Delay_ms(1);
    }
    return -RET_TIMEOUT;
}
//保护解除
int32_t Device_ZB35Q01_Unlock(void)
{
    uint8_t cmd[2] = {0x1F, 0xA0};
    uint8_t value = 0x00;
    int32_t ret;
    ret = platform_dev_spi_zb35q01.write_cmd(&platform_dev_spi_zb35q01, cmd, 2, &value, 1);
    if (ret != RET_OK) return ret;
    cmd[0] = 0x0F;
    value = 0xFF;
    ret = platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01, cmd, 2, &value, 1);
    if (ret != RET_OK) return ret;

    return (value & 0x38) ? -RET_ERROR : RET_OK;
}
int32_t Device_ZB35Q01_Init(void)
{
	int32_t ret;
	uint8_t reset_cmd = ZB35Q01XX_CMD_RESET;
	uint8_t status_cmd[2] = {0x0F, 0xC0};
	uint8_t status;
	uint8_t i=0;
	if (platform_dev_spi_zb35q01.write_cmd(&platform_dev_spi_zb35q01,&reset_cmd,1U,NULL,0U) != RET_OK)
	{
        return -RET_COMM_ERR;
    }
	Delay_ms(1U);
	do {
        ret = platform_dev_spi_zb35q01.read_cmd(&platform_dev_spi_zb35q01, status_cmd, 2, &status, 1);
		if(ret<0)
			return ret;
		Delay_ms(1U);
		if(++i>99)
			return -RET_TIMEOUT;
    } while (status & 0x01);
	ret = Device_ZB35Q01_ReadGeneral();
	if(ret<0)
		return ret;		
	ret = Device_ZB35Q01_Unlock();
	if(ret<0)
		return ret;	
	return RET_OK;
}


