/**
  ******************************************************************************
  * @file    Platform.h
  * @brief   平台层公共接口定义。
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
#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/*初始化和中断回调函数*/
typedef int32_t (*stmdev_init)(void*);
typedef int32_t (*stmdev_callback)(void*);
typedef int32_t (*stmdev_close)(void*);

/**
 * @brief 用于带键值设备的通用读写函数
 * @param 句柄    键值  数据  数据长度
*/
typedef int32_t (*stmdev_write_ptr)(void *, uint32_t, const void *, uint16_t);
typedef int32_t (*stmdev_read_ptr)(void *, uint32_t, void *, uint16_t);

/**
 * @brief 用于命令型设备的读写函数
 * @param 句柄    命令  命令长度    数据  数据长度
*/
typedef int32_t (*stmdev_write_cmd_ptr)(void *, const uint8_t *, uint16_t,
                                        const uint8_t *, uint16_t);
typedef int32_t (*stmdev_read_cmd_ptr)(void *, const uint8_t *, uint16_t,
                                       uint8_t *, uint16_t);

/**
 * @brief 用于带输出键值设备的接收函数
 * @param 句柄    键值输出  数据  数据长度输出
*/
typedef int32_t (*stmdev_receive_ptr)(void *, uint32_t *, void *, uint16_t *);

/**
 * @brief 用于单数据设备的读写函数
 * @param 句柄    数据
*/
typedef int32_t (*stmdev_write_value_ptr)(void *, uint32_t);
typedef int32_t (*stmdev_read_value_ptr)(void *, uint32_t *);


/*延时函数*/
typedef void (*stmdev_mdelay_ptr)(uint32_t millisec);

typedef struct
{
    stmdev_init        init;
    stmdev_callback    callback; 
    stmdev_close       close;
    
    stmdev_write_ptr  write;
    stmdev_read_ptr   read;

    stmdev_write_cmd_ptr write_cmd;
    stmdev_read_cmd_ptr read_cmd;

    stmdev_receive_ptr receive;

    stmdev_write_value_ptr write_value;
    stmdev_read_value_ptr  read_value;
    
    stmdev_mdelay_ptr   mdelay;

    void *handle;
    uint32_t io;
    /** private data **/
    void *priv_data;
    /** extern data **/
    void *extern_data;
} stmdev_ctx_t;
/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/



#ifdef __cplusplus
}
#endif
#endif   /*__PLATFORM_H__*/
