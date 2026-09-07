/**
  ******************************************************************************
  * @file           : Device_W25N01KV.c
  * @brief          : W25N01KV驱动层文件
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
#include "Device_W25N01KV.h"

/* Define ------------------------------------------------------------------*/

/* Variable ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/
/**
 * @brief  读取状态寄存器。
 * @note   手册时序为 0Fh + Register Address，然后返回 1 字节状态值。
 */
static int32_t w25n01kv_read_status(uint8_t reg_addr,
                                                   uint8_t *status)
{
    uint8_t cmd[2] = {W25N01KV_CMD_READ_STATUS, reg_addr};

    if (status == 0)
    {
        return -LY_ERR;
    }

    if (platform_spi_w25n01_dev.read_cmd(&platform_spi_w25n01_dev,
                                cmd,
                                sizeof(cmd),
                                status,
                                1U) != 0)
    {
        return -LY_ERR;
    }

    return LY_OK;
}

/**
 * @brief  写状态寄存器。
 * @note   手册时序为 1Fh + Register Address + Status Value。
 */
static int32_t w25n01kv_write_status(uint8_t reg_addr,
                                                    uint8_t status)
{
    uint8_t cmd[2] = {W25N01KV_CMD_WRITE_STATUS, reg_addr};

    if (platform_spi_w25n01_dev.write_cmd(&platform_spi_w25n01_dev,
                                 cmd,
                                 sizeof(cmd),
                                 &status,
                                 1U) != 0)
    {
        return -LY_ERR;
    }

    return LY_OK;
}

/**
 * @brief  等待芯片空闲。
 * @note   轮询 SR3 BUSY 位，BUSY 清零后芯片可接受下一条指令。
 */
static int32_t w25n01kv_wait_busy(uint8_t *status)
{
    uint8_t sr3 = 0U;

    for (uint32_t i = 0U; i < W25N01KV_BUSY_TIMEOUT_MS; i++)
    {
        if (w25n01kv_read_status(W25N01KV_SR3_ADDR, &sr3) != LY_OK)
        {
            return -LY_ERR;
        }

        if ((sr3 & W25N01KV_SR3_BUSY) == 0U)
        {
            if (status != 0)
            {
                *status = sr3;
            }
            return LY_OK;
        }

        platform_spi_w25n01_dev.mdelay(1U);
    }

    return -LY_ERR;
}

/**
 * @brief  写使能。
 * @note   写状态寄存器、擦除和页编程前都需要先设置 WEL 位。
 */
static int32_t w25n01kv_write_enable(void)
{
    uint8_t cmd = W25N01KV_CMD_WRITE_ENABLE;
    uint8_t status = 0U;

    if (platform_spi_w25n01_dev.write_cmd(&platform_spi_w25n01_dev,
                                 &cmd,
                                 1U,
                                 0,
                                 0U) != 0)
    {
        return -LY_ERR;
    }

    for (uint8_t i = 0U; i < 3U; i++)
    {
        platform_spi_w25n01_dev.mdelay(1U);
        if ((w25n01kv_read_status(W25N01KV_SR3_ADDR, &status) == LY_OK) &&
            ((status & W25N01KV_SR3_WEL) != 0U))
        {
            return LY_OK;
        }
    }

    return -LY_ERR;
}

/**
 * @brief  检查页地址、列地址和数据长度。
 * @note   当前接口只访问 2KB 主数据区，不跨页访问。
 */
static int32_t w25n01kv_check_page_range(uint32_t block_addr,
                                                        uint8_t page_in_block,
                                                        uint16_t column_addr,
                                                        const void *data,
                                                        uint16_t len)
{
    if ((data == 0) || (block_addr >= W25N01KV_BLOCK_COUNT) ||
        (page_in_block >= W25N01KV_PAGES_PER_BLOCK) ||
        (column_addr >= W25N01KV_PAGE_SIZE) ||
        ((uint32_t)column_addr + len > W25N01KV_PAGE_SIZE))
    {
        return -LY_ERR;
    }

    return LY_OK;
}

/**
 * @brief  复位芯片后读取并校验 JEDEC ID。
 * @note   W25N01KV 上电后先执行软件复位，等待 tRST 完成后再访问。
 */
int32_t W25N01KV_Init(void)
{
    uint8_t reset_cmd = W25N01KV_CMD_RESET;
    uint8_t id[W25N01KV_ID_LEN];
    uint32_t device_id;

    if (platform_spi_w25n01_dev.write_cmd(&platform_spi_w25n01_dev,
                                 &reset_cmd,
                                 1U,
                                 0,
                                 0U) != 0)
    {
        return -LY_ERR;
    }

    platform_spi_w25n01_dev.mdelay(1U);

    if (W25N01KV_ReadID(id) != LY_OK)
    {
        return -LY_ERR;
    }

    device_id = ((uint32_t)id[1] << 8) | id[2];
    if ((id[0] != W25N01KV_MFR_ID) || (device_id != W25N01KV_DEVICE_ID))
    {
        return -LY_ERR;
    }

    if (w25n01kv_wait_busy(0) != LY_OK)
    {
        return -LY_ERR;
    }

    if (W25N01KV_DisableBlockProtect() != LY_OK)
    {
        return -LY_ERR;
    }

    return LY_OK;
}

/**
 * @brief  读取 JEDEC ID。
 * @note   手册时序为 9Fh + 8 个 dummy clocks，然后返回 EFh AEh 21h。
 */
int32_t W25N01KV_ReadID(uint8_t id[W25N01KV_ID_LEN])
{
    uint8_t cmd[2] = {W25N01KV_CMD_READ_ID, 0x00U};

    if (id == 0)
    {
        return -LY_ERR;
    }

    if (platform_spi_w25n01_dev.read_cmd(&platform_spi_w25n01_dev,
                                cmd,
                                sizeof(cmd),
                                id,
                                W25N01KV_ID_LEN) != 0)
    {
        return -LY_ERR;
    }

    return LY_OK;
}

/**
 * @brief  解除块保护。
 * @note   写 SR1 为 0，清除 TB/BP3/BP2/BP1/BP0 保护位。
 */
int32_t W25N01KV_DisableBlockProtect(void)
{
    if (w25n01kv_write_enable() != LY_OK)
    {
        return -LY_ERR;
    }

    if (w25n01kv_write_status(W25N01KV_SR1_ADDR, 0x00U) != LY_OK)
    {
        return -LY_ERR;
    }

    return w25n01kv_wait_busy(0);
}

/**
 * @brief  擦除一个 128KB 块。
 * @note   手册时序为 Write Enable，然后 D8h + 24-bit Page Address。
 * @note   共 1024 个块，范围 0 ~ 1023；每块 64 页，每页 2048 bytes。
 */
int32_t W25N01KV_EraseBlock(uint32_t block_addr)
{
    uint32_t page_addr = block_addr * W25N01KV_PAGES_PER_BLOCK;
    uint8_t status = 0U;
    uint8_t cmd[4];

    if (block_addr >= W25N01KV_BLOCK_COUNT)
    {
        return -LY_ERR;
    }

    if (w25n01kv_read_status(W25N01KV_SR1_ADDR, &status) != LY_OK)
    {
        return -LY_ERR;
    }

    if (((status & W25N01KV_SR1_BP_MASK) != 0U) &&
        (W25N01KV_DisableBlockProtect() != LY_OK))
    {
        return -LY_ERR;
    }

    if (w25n01kv_write_enable() != LY_OK)
    {
        return -LY_ERR;
    }

    cmd[0] = W25N01KV_CMD_BLOCK_ERASE;
    cmd[1] = (uint8_t)(page_addr >> 16);
    cmd[2] = (uint8_t)(page_addr >> 8);
    cmd[3] = (uint8_t)page_addr;

    if (platform_spi_w25n01_dev.write_cmd(&platform_spi_w25n01_dev,
                                 cmd,
                                 sizeof(cmd),
                                 0,
                                 0U) != 0)
    {
        return -LY_ERR;
    }

    if (w25n01kv_wait_busy(&status) != LY_OK)
    {
        return -LY_ERR;
    }

    return ((status & W25N01KV_SR3_ERASE_FAIL) == 0U) ? LY_OK : -LY_ERR;
}

/**
 * @brief  从指定页读取数据。
 * @note   先用 13h 将页搬到 Data Buffer，再用 03h + Column Address + dummy clocks 读取。
 * @note   block_addr 范围 0 ~ 1023，page_in_block 范围 0 ~ 63。
 */
int32_t W25N01KV_ReadPage(uint32_t block_addr,
                                         uint8_t page_in_block,
                                         uint16_t column_addr,
                                         uint8_t *data,
                                         uint16_t len)
{
    uint32_t page_addr = block_addr * W25N01KV_PAGES_PER_BLOCK + page_in_block;
    uint8_t status = 0U;
    uint8_t read_page_cmd[4];
    uint8_t read_data_cmd[4];

    if (len == 0U)
    {
        return LY_OK;
    }

    if (w25n01kv_check_page_range(block_addr, page_in_block, column_addr, data, len) != LY_OK)
    {
        return -LY_ERR;
    }

    (void)w25n01kv_read_status(W25N01KV_SR3_ADDR, &status);

    read_page_cmd[0] = W25N01KV_CMD_PAGE_DATA_READ;
    read_page_cmd[1] = (uint8_t)(page_addr >> 16);
    read_page_cmd[2] = (uint8_t)(page_addr >> 8);
    read_page_cmd[3] = (uint8_t)page_addr;

    if (platform_spi_w25n01_dev.write_cmd(&platform_spi_w25n01_dev,
                                 read_page_cmd,
                                 sizeof(read_page_cmd),
                                 0,
                                 0U) != 0)
    {
        return -LY_ERR;
    }

    platform_spi_w25n01_dev.mdelay(1U);

    if (w25n01kv_wait_busy(&status) != LY_OK)
    {
        return -LY_ERR;
    }

    if ((status & W25N01KV_SR3_ECC_MASK) == W25N01KV_SR3_ECC_UNCORRECT)
    {
        return -LY_ERR;
    }

    read_data_cmd[0] = W25N01KV_CMD_READ_DATA;
    read_data_cmd[1] = (uint8_t)(column_addr >> 8);
    read_data_cmd[2] = (uint8_t)column_addr;
    read_data_cmd[3] = 0x00U;

    if (platform_spi_w25n01_dev.read_cmd(&platform_spi_w25n01_dev,
                                read_data_cmd,
                                sizeof(read_data_cmd),
                                data,
                                len) != 0)
    {
        return -LY_ERR;
    }

    return LY_OK;
}

/**
 * @brief  向已擦除页写入数据。
 * @note   先用 02h 写入 Data Buffer，再用 10h + 24-bit Page Address 执行页编程。
 * @note   block_addr 范围 0 ~ 1023，page_in_block 范围 0 ~ 63。
 */
int32_t W25N01KV_WritePage(uint32_t block_addr,
                                          uint8_t page_in_block,
                                          uint16_t column_addr,
                                          const uint8_t *data,
                                          uint16_t len)
{
    uint32_t page_addr = block_addr * W25N01KV_PAGES_PER_BLOCK + page_in_block;
    uint8_t status = 0U;
    uint8_t id[W25N01KV_ID_LEN] = {0U};
    uint8_t sr1 = 0U;
    uint8_t sr3 = 0U;
    uint8_t load_cmd[3];
    uint8_t execute_cmd[4];

    if (len == 0U)
    {
        return LY_OK;
    }

    if (w25n01kv_check_page_range(block_addr, page_in_block, column_addr, data, len) != LY_OK)
    {
        return -LY_ERR;
    }

    /* LOAD 前先读一次状态，避免空闲一段时间后首个写使能不生效。 */
    w25n01kv_read_status(W25N01KV_SR3_ADDR, &sr3);

    if (w25n01kv_write_enable() != LY_OK)
    {
        return -LY_ERR;
    }

    load_cmd[0] = W25N01KV_CMD_LOAD_PROGRAM_DATA;
    load_cmd[1] = (uint8_t)(column_addr >> 8);
    load_cmd[2] = (uint8_t)column_addr;

    if (platform_spi_w25n01_dev.write_cmd(&platform_spi_w25n01_dev,
                                 load_cmd,
                                 sizeof(load_cmd),
                                 data,
                                 len) != 0)
    {
        return -LY_ERR;
    }

    if (w25n01kv_write_enable() != LY_OK)
    {
        return -LY_ERR;
    }

    execute_cmd[0] = W25N01KV_CMD_PROGRAM_EXECUTE;
    execute_cmd[1] = (uint8_t)(page_addr >> 16);
    execute_cmd[2] = (uint8_t)(page_addr >> 8);
    execute_cmd[3] = (uint8_t)page_addr;

    if (platform_spi_w25n01_dev.write_cmd(&platform_spi_w25n01_dev,
                                 execute_cmd,
                                 sizeof(execute_cmd),
                                 0,
                                 0U) != 0)
    {
        return -LY_ERR;
    }
    if (w25n01kv_wait_busy(&status) != LY_OK)
    {
        return -LY_ERR;
    }

    return ((status & W25N01KV_SR3_PROGRAM_FAIL) == 0U) ? LY_OK : (-LY_ERR);
}

