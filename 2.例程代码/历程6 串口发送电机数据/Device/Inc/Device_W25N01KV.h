/**
  ******************************************************************************
  * @file    Device_W25N01KV.h
  * @brief   W25N01KV驱动层头文件。
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
#ifndef __DEVICE_W25N01KV_H__
#define __DEVICE_W25N01KV_H__
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "Platform_SPI.h"
/* Private defines -----------------------------------------------------------*/
/* JEDEC ID: Manufacturer ID + 2-byte Device ID. */
#define W25N01KV_ID_LEN    3U
#define W25N01KV_MFR_ID    0xEFU
#define W25N01KV_DEVICE_ID 0xAE21U

/* Main array: 1024 blocks * 64 pages * 2048 bytes = 128 MiB. */
#define W25N01KV_PAGE_SIZE       2048U
#define W25N01KV_PAGES_PER_BLOCK 64U
#define W25N01KV_BLOCK_COUNT     1024U
#define W25N01KV_PAGE_COUNT      (W25N01KV_BLOCK_COUNT * W25N01KV_PAGES_PER_BLOCK)

#define W25N01KV_CMD_RESET             0xFFU
#define W25N01KV_CMD_READ_ID           0x9FU
#define W25N01KV_CMD_READ_STATUS       0x0FU
#define W25N01KV_CMD_WRITE_STATUS      0x1FU
#define W25N01KV_CMD_WRITE_ENABLE      0x06U
#define W25N01KV_CMD_BLOCK_ERASE       0xD8U
#define W25N01KV_CMD_LOAD_PROGRAM_DATA 0x02U
#define W25N01KV_CMD_PROGRAM_EXECUTE   0x10U
#define W25N01KV_CMD_PAGE_DATA_READ    0x13U
#define W25N01KV_CMD_READ_DATA         0x03U

#define W25N01KV_SR1_ADDR 0xA0U
#define W25N01KV_SR3_ADDR 0xC0U

#define W25N01KV_SR1_BP_MASK        0x7CU
#define W25N01KV_SR3_BUSY           0x01U
#define W25N01KV_SR3_WEL            0x02U
#define W25N01KV_SR3_ERASE_FAIL     0x04U
#define W25N01KV_SR3_PROGRAM_FAIL   0x08U
#define W25N01KV_SR3_ECC_MASK       0x30U
#define W25N01KV_SR3_ECC_UNCORRECT  0x20U

#define W25N01KV_BUSY_TIMEOUT_MS 1000U
/* Exported types ------------------------------------------------------------*/


/* Exported extern variables -------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
int32_t W25N01KV_Init(void);
int32_t W25N01KV_ReadID(uint8_t id[W25N01KV_ID_LEN]);
int32_t W25N01KV_DisableBlockProtect(void);
int32_t W25N01KV_EraseBlock(uint32_t block_addr);
int32_t W25N01KV_ReadPage(uint32_t block_addr,
                                         uint8_t page_in_block,
                                         uint16_t column_addr,
                                         uint8_t *data,
                                         uint16_t len);
int32_t W25N01KV_WritePage(uint32_t block_addr,
                                          uint8_t page_in_block,
                                          uint16_t column_addr,
                                          const uint8_t *data,
                                          uint16_t len);
#ifdef __cplusplus
}
#endif
#endif   /*__DEVICE_W25N01KV_H__*/
