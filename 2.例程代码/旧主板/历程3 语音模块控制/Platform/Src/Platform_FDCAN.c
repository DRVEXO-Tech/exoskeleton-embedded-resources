/**
  ******************************************************************************
  * @file           : Platform_FDCAN.c
  * @brief          : FDCAN平台层文件
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
#include "Platform_FDCAN.h"

/* Define ------------------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/
static uint32_t fdcan_len_to_dlc(uint8_t len);
static uint8_t fdcan_dlc_to_len(uint32_t dlc);
static int32_t platform_fdcan_init(void *ctx);
static int32_t platform_fdcan_callback(void *ctx);
static int32_t platform_fdcan_write(void *ctx, uint32_t key, const void *bufp, uint16_t len);
static int32_t platform_fdcan_read(void *ctx, uint32_t *key, void *bufp, uint16_t *len);
static int32_t platform_fdcan_config_interrupts(FDCAN_HandleTypeDef *handle);
static int32_t platform_fdcan_activate_notifications(FDCAN_HandleTypeDef *handle);

/* Variable ------------------------------------------------------------------*/
struct platform_priv_data_t platform_priv_data;
struct platform_extern_data_t platform_extern_data;
static volatile uint8_t platform_fdcan_busoff_pending = 0U;
stmdev_ctx_t platform_fdcan_dev =
{
    .handle = &hfdcan1,
    .write = platform_fdcan_write,
    .receive = platform_fdcan_read,
    .init = platform_fdcan_init,
    .callback = platform_fdcan_callback,
    .priv_data = &platform_priv_data,
    .extern_data = &platform_extern_data,
};

/* Function ------------------------------------------------------------------*/

/**
* @brief FDCAN数据长度转为DLC
* @param len 数据长度
* @return DLC
*/
static uint32_t fdcan_len_to_dlc(uint8_t len)
{
    switch (len)
    {
    case 0:  return FDCAN_DLC_BYTES_0;
    case 1:  return FDCAN_DLC_BYTES_1;
    case 2:  return FDCAN_DLC_BYTES_2;
    case 3:  return FDCAN_DLC_BYTES_3;
    case 4:  return FDCAN_DLC_BYTES_4;
    case 5:  return FDCAN_DLC_BYTES_5;
    case 6:  return FDCAN_DLC_BYTES_6;
    case 7:  return FDCAN_DLC_BYTES_7;
    case 8:  return FDCAN_DLC_BYTES_8;
    case 12: return FDCAN_DLC_BYTES_12;
    case 16: return FDCAN_DLC_BYTES_16;
    case 20: return FDCAN_DLC_BYTES_20;
    case 24: return FDCAN_DLC_BYTES_24;
    case 32: return FDCAN_DLC_BYTES_32;
    case 48: return FDCAN_DLC_BYTES_48;
    case 64: return FDCAN_DLC_BYTES_64;
    default: return 0xFFFFFFFF;
    }
}

/**
* @brief FDCAN的DLC转为数据长度
* @param dlc DLC
* @return 数据长度
*/
static uint8_t fdcan_dlc_to_len(uint32_t dlc)
{
    switch (dlc)
    {
    case FDCAN_DLC_BYTES_0:  return 0;
    case FDCAN_DLC_BYTES_1:  return 1;
    case FDCAN_DLC_BYTES_2:  return 2;
    case FDCAN_DLC_BYTES_3:  return 3;
    case FDCAN_DLC_BYTES_4:  return 4;
    case FDCAN_DLC_BYTES_5:  return 5;
    case FDCAN_DLC_BYTES_6:  return 6;
    case FDCAN_DLC_BYTES_7:  return 7;
    case FDCAN_DLC_BYTES_8:  return 8;
    case FDCAN_DLC_BYTES_12: return 12;
    case FDCAN_DLC_BYTES_16: return 16;
    case FDCAN_DLC_BYTES_20: return 20;
    case FDCAN_DLC_BYTES_24: return 24;
    case FDCAN_DLC_BYTES_32: return 32;
    case FDCAN_DLC_BYTES_48: return 48;
    case FDCAN_DLC_BYTES_64: return 64;
    default: return 0;
    }
}

static int32_t platform_fdcan_config_interrupts(FDCAN_HandleTypeDef *handle)
{
    if (handle == 0)
    {
        return -LY_ERR_INVALID;
    }

    if (HAL_FDCAN_ConfigInterruptLines(handle, FDCAN_IT_GROUP_RX_FIFO0, FDCAN_INTERRUPT_LINE0) != HAL_OK)
    {
        return -LY_ERR_IO;
    }
    if (HAL_FDCAN_ConfigInterruptLines(handle, FDCAN_IT_GROUP_PROTOCOL_ERROR, FDCAN_INTERRUPT_LINE1) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    return LY_OK;
}

static int32_t platform_fdcan_activate_notifications(FDCAN_HandleTypeDef *handle)
{
    if (handle == 0)
    {
        return -LY_ERR_INVALID;
    }

    if (HAL_FDCAN_ActivateNotification(handle, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0U) != HAL_OK)
    {
        return -LY_ERR_IO;
    }
    if (HAL_FDCAN_ActivateNotification(handle, FDCAN_IT_BUS_OFF, 0U) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    return LY_OK;
}

/**
* @brief 初始化FDCAN平台层
* @param ctx FDCAN设备上下文
* @return 初始化结果
*/
static int32_t platform_fdcan_init(void *ctx)
{
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    FDCAN_FilterTypeDef fdcan_filter = {0};

    if (dev == 0)
    {
        return -LY_ERR_INVALID;
    }

    memset(dev->priv_data, 0, sizeof(struct platform_priv_data_t));
    memset(dev->extern_data, 0, sizeof(struct platform_extern_data_t));

    fdcan_filter.IdType = FDCAN_STANDARD_ID;
    fdcan_filter.FilterIndex = 0;
    fdcan_filter.FilterType = FDCAN_FILTER_MASK;
    fdcan_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    fdcan_filter.FilterID1 = 0x303;
    fdcan_filter.FilterID2 = 0xFFF;
    if (HAL_FDCAN_ConfigFilter(dev->handle, &fdcan_filter) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    fdcan_filter.FilterIndex = 1;
    fdcan_filter.FilterID1 = 0x000;
    fdcan_filter.FilterID2 = 0xF00;
    if (HAL_FDCAN_ConfigFilter(dev->handle, &fdcan_filter) != HAL_OK)
    {
        return -LY_ERR_IO;
    }
    
    fdcan_filter.FilterIndex = 2;
    fdcan_filter.FilterID1 = 0x400;
    fdcan_filter.FilterID2 = 0xF00;
    if (HAL_FDCAN_ConfigFilter(dev->handle, &fdcan_filter) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    if (HAL_FDCAN_ConfigGlobalFilter(dev->handle,
                                     FDCAN_REJECT,
                                     FDCAN_REJECT,
                                     FDCAN_REJECT_REMOTE,
                                     FDCAN_REJECT_REMOTE) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    if (platform_fdcan_config_interrupts(dev->handle) != LY_OK)
    {
        return -LY_ERR_IO;
    }

    if (HAL_FDCAN_Start(dev->handle) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    if (platform_fdcan_activate_notifications(dev->handle) != LY_OK)
    {
        return -LY_ERR_IO;
    }

    return LY_OK;
}

int32_t Platform_FDCAN_RecoverBusOff(void)
{
    FDCAN_ProtocolStatusTypeDef protocol_status = {0};

    if (HAL_FDCAN_GetProtocolStatus(&hfdcan1, &protocol_status) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    if ((platform_fdcan_busoff_pending == 0U) && (protocol_status.BusOff == 0U))
    {
        return LY_OK;
    }

    platform_fdcan_busoff_pending = 0U;

    if (HAL_FDCAN_Stop(&hfdcan1) != HAL_OK)
    {
        platform_fdcan_busoff_pending = 1U;
        return -LY_ERR_IO;
    }

    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        platform_fdcan_busoff_pending = 1U;
        return -LY_ERR_IO;
    }

    if (platform_fdcan_activate_notifications(&hfdcan1) != LY_OK)
    {
        platform_fdcan_busoff_pending = 1U;
        return -LY_ERR_IO;
    }

    return LY_OK;
}


/**
* @brief 发送一帧FDCAN数据
* @param handle FDCAN句柄
* @param id 标准帧ID
* @param bufp 发送数据指针
* @param len 发送数据长度
* @return 发送结果
*/
static int32_t platform_fdcan_write(void *ctx, uint32_t key, const void *bufp, uint16_t len)
{
    FDCAN_TxHeaderTypeDef tx_header = {0};
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    const uint8_t *data = (const uint8_t *)bufp;
    uint32_t dlc;

    if ((dev == 0) || ((bufp == 0) && (len != 0)) || (key > 0x7FF) || (len > PLATFORM_FDCAN_DATA_MAX_LEN))
    {
        return -LY_ERR_INVALID;
    }

    dlc = fdcan_len_to_dlc((uint8_t)len);
    if (dlc == 0xFFFFFFFF)
    {
        return -LY_ERR_INVALID;
    }

    tx_header.Identifier = key;
    tx_header.IdType = FDCAN_STANDARD_ID;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.DataLength = dlc;
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_ON;
    tx_header.FDFormat = FDCAN_FD_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0U;

    if (HAL_FDCAN_AddMessageToTxFifoQ(dev->handle, &tx_header, data) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    return LY_OK;
}


/**
* @brief 读取一帧FDCAN数据
* @param handle FDCAN句柄
* @param id 接收帧ID输出指针
* @param bufp 接收数据缓冲区
* @param len 接收数据长度输出指针
* @return 读取结果
*/
static int32_t platform_fdcan_read(void *ctx, uint32_t *key, void *bufp, uint16_t *len)
{
    FDCAN_RxHeaderTypeDef rx_header = {0};
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    uint8_t *data = (uint8_t *)bufp;

    if ((dev == 0) || (key == 0) || (bufp == 0) || (len == 0))
    {
        return -LY_ERR_INVALID;
    }

    memset(data, 0, PLATFORM_FDCAN_DATA_MAX_LEN);
    if (HAL_FDCAN_GetRxMessage(dev->handle, FDCAN_RX_FIFO0, &rx_header, data) != HAL_OK)
    {
        return -LY_ERR_IO;
    }

    *len = fdcan_dlc_to_len(rx_header.DataLength);
    *key = rx_header.Identifier;
    return LY_OK;
}


/**
* @brief 处理FDCAN接收回调并分类缓存数据
* @param ctx FDCAN设备上下文
* @return 处理结果
*/
static int32_t platform_fdcan_callback(void *ctx)
{
    stmdev_ctx_t *dev = (stmdev_ctx_t *)ctx;
    struct platform_priv_data_t *priv_data;
    struct platform_extern_data_t *extern_data;
    uint32_t now_tick;

    if (dev == 0)
    {
        return -LY_ERR_INVALID;
    }

    priv_data = (struct platform_priv_data_t *)dev->priv_data;
    extern_data = (struct platform_extern_data_t *)dev->extern_data;

    while (HAL_FDCAN_GetRxFifoFillLevel(dev->handle, FDCAN_RX_FIFO0) > 0)
    {
        memset(priv_data->receive_data, 0, sizeof(priv_data->receive_data));
        if (dev->receive(dev, &priv_data->id, priv_data->receive_data, &priv_data->rx_len) != LY_OK)
        {
            return -LY_ERR_IO;
        }

        switch (priv_data->id)
        {
        case 0x303:
            memset(extern_data->Batter_buf, 0, sizeof(extern_data->Batter_buf));
            memcpy(extern_data->Batter_buf, priv_data->receive_data, PLATFORM_FDCAN_BAT_BUF_LEN);
            sys_ready.Bat_Ready =1;
            break;

        case 0x000:
            if (priv_data->rx_len == PLATFORM_FDCAN_MOTOR_BUF_LEN)
            {
                now_tick = HAL_GetTick();
                if (extern_data->Leftmotor_last_tick_ms != 0U)
                {
                    extern_data->Leftmotor_dt_ms = now_tick - extern_data->Leftmotor_last_tick_ms;
                }
                else
                {
                    extern_data->Leftmotor_dt_ms = 0U;
                }
                extern_data->Leftmotor_last_tick_ms = now_tick;
                memset(extern_data->Leftmotor_buf, 0, sizeof(extern_data->Leftmotor_buf));
                memcpy(extern_data->Leftmotor_buf, priv_data->receive_data, PLATFORM_FDCAN_MOTOR_BUF_LEN);
                sys_ready.Left_Motor_Ready = 1;
            }
            break;

        case 0x001:
            if (priv_data->rx_len == PLATFORM_FDCAN_MOTOR_BUF_LEN)
            {
                now_tick = HAL_GetTick();
                if (extern_data->Rightmotor_last_tick_ms != 0U)
                {
                    extern_data->Rightmotor_dt_ms = now_tick - extern_data->Rightmotor_last_tick_ms;
                }
                else
                {
                    extern_data->Rightmotor_dt_ms = 0U;
                }
                extern_data->Rightmotor_last_tick_ms = now_tick;
                memset(extern_data->Rightmotor_buf, 0, sizeof(extern_data->Rightmotor_buf));
                memcpy(extern_data->Rightmotor_buf, priv_data->receive_data, PLATFORM_FDCAN_MOTOR_BUF_LEN);
                sys_ready.Right_Motor_Ready = 1;
            }
            break;

        case 0x002:
            if (priv_data->rx_len > 0U)
            {
                extern_data->Leftmotor_receive_one_data = priv_data->receive_data[0];
                extern_data->Leftmotor_receive_flag = 0U;
            }
            else
            {
                extern_data->Leftmotor_receive_one_data = 0U;
                extern_data->Leftmotor_receive_flag = 0U;
            }
            break;

        case 0x003:
            if (priv_data->rx_len > 0U)
            {
                extern_data->Rightmotor_receive_one_data = priv_data->receive_data[0];
                extern_data->Rightmotor_receive_flag = 0U;
            }
            else
            {
                extern_data->Rightmotor_receive_one_data = 0U;
                extern_data->Rightmotor_receive_flag = 0U;
            }
            break;
        default:
            break;
        }
    }

    return LY_OK;
}

void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
    if ((hfdcan == &hfdcan1) && ((ErrorStatusITs & FDCAN_IT_BUS_OFF) != 0U))
    {
        platform_fdcan_busoff_pending = 1U;
    }
}
