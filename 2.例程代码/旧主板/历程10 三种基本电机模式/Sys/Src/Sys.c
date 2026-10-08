/**
  ******************************************************************************
  * @file           : Sys.c
  * @brief          : 系统初始化和用户配置文件
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
#include "Sys.h"
#include "DRVEXO_IMU.h"
#include "APP_ISM6DSO.h"
#include "APP_LPS27HHTW.h"
#include "DRVEXO_Scheduler.h"
#include "APP_FDCAN.h"
#include "APP_LowPower.h"
#include "Device_IO_Control.h"
#include "Device_W25N01KV.h"
#include "APP_Control.h"
#include "APP_VOICE.h"
#include "User_Data.h"
/* Define ------------------------------------------------------------------*/
#define SYS_BATTERY_VOICE_WAIT_TIMEOUT_MS  1000U
#define SYS_BATTERY_DATA_SYNC_DELAY_MS     2U
#define SYS_USRCONFIG_WRITE_ALIGN      16U
#define SYS_USRCONFIG_FLASH_RETRY      3U

/* Variable ------------------------------------------------------------------*/
struct sys_err_t sys_err;
struct sys_ready_t sys_ready;
struct Firmware_Version_Data_st Firmware_Version_data;

/* Function ------------------------------------------------------------------*/

/**
* @brief 将三段版本号编码为32位版本值
* @param v1 主版本号
* @param v2 次版本号
* @param v3 修订版本号
* @return 32位版本值
*/
static uint32_t sys_firmware_version_get(uint16_t v1, uint16_t v2, uint16_t v3)
{
    return (((uint32_t)v1 & 0x03FFU) << 22U) |
           (((uint32_t)v2 & 0x03FFU) << 12U) |
           ((uint32_t)v3 & 0x0FFFU);
}

/**
* @brief 清除内部Flash操作标志，避免残留错误影响下一次擦写
* @param 无
* @return 无
*/
static void Sys_UsrConfig_ClearFlashFlag(void)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS | FLASH_FLAG_EOP);
}

/**
* @brief 从内部Flash读取用户数据
* @param data 用户数据输出缓存
* @param length 读取长度，不能超过保留Flash区域
* @return LY_OK表示成功，负值表示失败
*/
int32_t Sys_UsrConfig_Read(void *data, uint32_t length)
{
    if((data == NULL) || (length == 0U) || (length > USRCONFIG_SIZE))
    {
        return -LY_ERR_INVALID;
    }

    memcpy(data, (const void *)USRCONFIG_START_ADDR, length);

    return LY_OK;
}

/**
* @brief 擦除用户数据Flash区域
* @param 无
* @return LY_OK表示成功，负值表示失败
*/
int32_t Sys_UsrConfig_Erase(void)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t sector_error = 0xFFFFFFFFU;
    uint32_t retry;
    int32_t ret = LY_OK;

    memset(&erase, 0, sizeof(erase));
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Banks = USRCONFIG_BANK;
    erase.Sector = USRCONFIG_SECTOR;
    erase.NbSectors = 1U;

    if(HAL_FLASH_Unlock() != HAL_OK)
    {
        return -LY_ERR;
    }

    ret = -LY_ERR;
    for(retry = 0U; retry < SYS_USRCONFIG_FLASH_RETRY; retry++)
    {
        sector_error = 0xFFFFFFFFU;
        Sys_UsrConfig_ClearFlashFlag();
        if(HAL_FLASHEx_Erase(&erase, &sector_error) == HAL_OK)
        {
            ret = LY_OK;
            break;
        }
    }

    if(HAL_FLASH_Lock() != HAL_OK)
    {
        ret = -LY_ERR;
    }

    return ret;
}

/**
* @brief 将用户数据写入内部Flash
* @param data 待写入数据
* @param length 写入长度，不能超过保留Flash区域
* @return LY_OK表示成功，负值表示失败
*/
int32_t Sys_UsrConfig_Write(const void *data, uint32_t length)
{
    union
    {
        uint32_t word[SYS_USRCONFIG_WRITE_ALIGN / sizeof(uint32_t)];
        uint8_t buf[SYS_USRCONFIG_WRITE_ALIGN];
    } write_block;
    const uint8_t *source = (const uint8_t *)data;
    uint32_t block_length;
    uint32_t offset;
    int32_t ret = LY_OK;

    if((data == NULL) || (length == 0U) || (length > USRCONFIG_SIZE))
    {
        return -LY_ERR_INVALID;
    }

    if(HAL_FLASH_Unlock() != HAL_OK)
    {
        return -LY_ERR;
    }

    Sys_UsrConfig_ClearFlashFlag();
    for(offset = 0U; offset < length; offset += SYS_USRCONFIG_WRITE_ALIGN)
    {
        block_length = length - offset;
        if(block_length > SYS_USRCONFIG_WRITE_ALIGN)
        {
            block_length = SYS_USRCONFIG_WRITE_ALIGN;
        }
        memset(write_block.buf, 0xFF, sizeof(write_block.buf));
        memcpy(write_block.buf, &source[offset], block_length);
        Sys_UsrConfig_ClearFlashFlag();
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD,
                             USRCONFIG_START_ADDR + offset,
                             (uint32_t)write_block.word) != HAL_OK)
        {
            ret = -LY_ERR;
            break;
        }
    }

    if(HAL_FLASH_Lock() != HAL_OK)
    {
        ret = -LY_ERR;
    }

    return ret;
}

/**
* @brief 系统初始化
* @param 无
* @return LY_OK表示成功
*/
int Sys_Init(void)
{
    uint8_t current_core_error_status;
    uint8_t last_core_error_status = 0xFFU;
    uint32_t core_error_send_tick = 0U;
    uint32_t battery_wait_start_tick;

    if(APP_LowPower_EarlyWakeConfirm() != LY_OK)
    {
        sys_err.LowPower_Init_err_cnt++;
        return -LY_ERR;
    }

    memset(&Firmware_Version_data, 0, sizeof(Firmware_Version_data));
    Firmware_Version_data.core.version = sys_firmware_version_get(APP_VERSION_0,
                                                                   APP_VERSION_1,
                                                                   APP_VERSION_2);
    Firmware_Version_data.core.version_buf[0] = APP_VERSION_0;
    Firmware_Version_data.core.version_buf[1] = APP_VERSION_1;
    Firmware_Version_data.core.version_buf[2] = APP_VERSION_2;

    if(Device_EN_5V_Write(1U) != LY_OK)
    {
        sys_err.LowPower_IO_err_cnt++;
    }
    if(Device_EN_3V3_Write(1U) != LY_OK)
    {
        sys_err.LowPower_IO_err_cnt++;
    }
    if(Device_EN_VM_Write(1U) != LY_OK)
    {
        sys_err.LowPower_IO_err_cnt++;
    }
    HAL_Delay(100);

    int8_t ret = 0;
    IMU_Data_Init();
    ret = ISM6DSO_Init();
    if(ret == 0)
    {
        Sub_IMU_Flag = 0;
        sys_ready.IMU_Ready = 1;
    }
    
    else if(ret == 1)
    {
        Sub_IMU_Flag = 1;
        sys_ready.IMU_Ready = 1;
    }
    else
        sys_err.IMU_Init_err_cnt++;
    if(LPS27HHTx_Init()!=0)
    {
        sys_err.Barometer_Init_err_cnt++;
    }
    else
    {
        sys_ready.Baro_Ready = 1;
    }
    HAL_GPIO_WritePin(FLASH_WP_GPIO_Port,FLASH_WP_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(FLASH_HOLD_GPIO_Port,FLASH_HOLD_Pin,GPIO_PIN_SET);
    if(W25N01KV_Init() != LY_OK)
    {
        sys_err.Flash_Init_err_cnt++;
    }
    else
    {
        sys_ready.Flash_Ready = 1U;
    }
    
    if(APP_FDCAN_Init() != LY_OK)
    {
        sys_err.FDCAN_Init_err_cnt++;
    }
    if(APP_LowPower_Init() != LY_OK)
    {
        sys_err.LowPower_Init_err_cnt++;
        return -LY_ERR;
    }
    Scheduler_Init();
	HAL_Delay(3000);
    HAL_TIM_Base_Start_IT(&htim7);

    if (APP_Control_StartAlgorithmCalibrationLight(100U) != LY_OK)
    {
        sys_err.Motor_Calibration_Pass_Notify_err_cnt++;
    }

    /* 等待首帧有效电池数据并在进入用户主函数前播报，未连接电池时超时继续启动。 */
    battery_wait_start_tick = HAL_GetTick();
    while ((sys_ready.Bat_Ready == 0U) &&
           ((uint32_t)(HAL_GetTick() - battery_wait_start_tick) < SYS_BATTERY_VOICE_WAIT_TIMEOUT_MS))
    {
        HAL_Delay(1U);
    }
    if (sys_ready.Bat_Ready != 0U)
    {
        HAL_Delay(SYS_BATTERY_DATA_SYNC_DELAY_MS);
        User_Battery_Level_Update();
        if (APP_Voice_PlayBatteryLevel(User_Battery_Level) != LY_OK)
        {
            sys_err.Voice_Play_err_cnt++;
        }
    }

    /* 后台任务已启动，未连接的可选外设不阻塞用户主入口。 */
    sys_ready.all_ready = 1U;
    return LY_OK;
}
