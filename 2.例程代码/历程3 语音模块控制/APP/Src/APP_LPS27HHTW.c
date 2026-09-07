/**
  ******************************************************************************
  * @file           : APP_LPS27HHTW.c
  * @brief          : LPS27HHTW APP层文件
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
#include "APP_LPS27HHTW.h"
#include "User_Data.h"

/* Define ------------------------------------------------------------------*/
#define SEA_LEVEL_PRESSURE_HPA (1013.25f)
#define CALIBRATION_WAIT_MS 1000U
#define CALIBRATION_SAMPLES 40U
#define ALTITUDE_AVG_WINDOW 5U
#define BASELINE_STATIONARY_DELAY_MS 3000U
#define BASELINE_TRACKING_ALPHA 0.001667f

/* Variable ------------------------------------------------------------------*/
float temperature_degC_LPS27;
float altitude_m = 0.0f;
uint8_t barometer_altitude_ready = 0U;
uint8_t g_baro_drdy_flag = 0;



static uint8_t whoamI;
float_t pressure_hPa;
static int32_t data_raw_pressure;
static int16_t data_raw_temperature;
static uint16_t init_count;
static float pressure_sum;
static float reference_pressure_hPa = SEA_LEVEL_PRESSURE_HPA;
static uint8_t is_calibrated;
static uint32_t calibration_start_tick;
static uint8_t baseline_tracking_enable;
static uint8_t baseline_stationary_timing;
static uint32_t baseline_stationary_start_tick;


//平均滤波
typedef struct
{
    float buffer[ALTITUDE_AVG_WINDOW];
    float sum;
    uint8_t index;
    uint8_t count;
} altitude_avg_filter_t;

static altitude_avg_filter_t s_altitude_avg_filter = {0};

/* Function ------------------------------------------------------------------*/
/**
* @brief 复位气压计运行时状态
* @param 无
* @return 无
*/
static void lps27hhtw_reset_runtime_state(void)
{
    data_raw_pressure = 0U;
    data_raw_temperature = 0;
    pressure_hPa = 0.0f;
    temperature_degC_LPS27 = 0.0f;
    altitude_m = 0.0f;
    barometer_altitude_ready = 0U;
    whoamI = 0U;
    g_baro_drdy_flag = 0U;
    init_count = 0U;
    pressure_sum = 0.0f;
    reference_pressure_hPa = SEA_LEVEL_PRESSURE_HPA;
    is_calibrated = 0U;
    calibration_start_tick = 0U;
    baseline_tracking_enable = 0U;
    baseline_stationary_timing = 0U;
    baseline_stationary_start_tick = 0U;
    memset(&s_altitude_avg_filter, 0, sizeof(s_altitude_avg_filter));
}

/**
* @brief 更新高度均值滤波结果
* @param sample 当前高度样本
* @return 滤波后高度
*/
static float altitude_average_filter_update(float sample)
{
    altitude_avg_filter_t *filter = &s_altitude_avg_filter;

    if (filter->count < ALTITUDE_AVG_WINDOW)
    {
        filter->buffer[filter->index] = sample;
        filter->sum += sample;
        filter->count++;
    }
    else
    {
        filter->sum -= filter->buffer[filter->index];
        filter->buffer[filter->index] = sample;
        filter->sum += sample;
    }

    filter->index++;
    if (filter->index >= ALTITUDE_AVG_WINDOW)
    {
        filter->index = 0;
    }

    return filter->sum / (float)filter->count;
}


/**
* @brief 初始化LPS27HHTW气压计
* @param 无
* @return 初始化结果
*/
int16_t LPS27HHTx_Init(void)
{
    lps22df_id_t id = {0};
    lps22df_bus_mode_t bus_mode = {0};
    lps22df_md_t md = {0};
    lps22df_pin_conf_t pin_conf = {0};
    lps22df_int_mode_t int_mode = {0};
    lps22df_pin_int_route_t int_route = {0};
    int32_t ret;

    lps27hhtw_reset_runtime_state();
    ret = lps22df_id_get(&platform_spi_lps22df_dev, &id);
    whoamI = id.whoami;

    if ((ret != 0) || (whoamI != LPS22DF_ID))
    {
        return -1;
    }

    ret = lps22df_init_set(&platform_spi_lps22df_dev, LPS22DF_BOOT);
    if (ret != 0)
    {
        return -1;
    }

    ret = lps22df_init_set(&platform_spi_lps22df_dev, LPS22DF_RESET);
    if (ret != 0)
    {
        return -1;
    }

    ret = lps22df_init_set(&platform_spi_lps22df_dev, LPS22DF_DRV_RDY);
    if (ret != 0)
    {
        return -1;
    }

    bus_mode.interface = LPS22DF_SEL_BY_HW;
    bus_mode.filter = LPS22DF_FILTER_AUTO;
    bus_mode.i3c_ibi_time = LPS22DF_IBI_50us;
    ret = lps22df_bus_mode_set(&platform_spi_lps22df_dev, &bus_mode);
    if (ret != 0)
    {
        return -1;
    }

    md.odr = LPS22DF_10Hz;
    md.avg = LPS22DF_16_AVG;
    md.lpf = LPS22DF_LPF_ODR_DIV_4;
    ret = lps22df_mode_set(&platform_spi_lps22df_dev, &md);
    if (ret != 0)
    {
        return -1;
    }

    ret = lps22df_pin_conf_get(&platform_spi_lps22df_dev, &pin_conf);
    if (ret != 0)
    {
        return -1;
    }
    pin_conf.int_push_pull = PROPERTY_ENABLE;
    ret = lps22df_pin_conf_set(&platform_spi_lps22df_dev, &pin_conf);
    if (ret != 0)
    {
        return -1;
    }

    ret = lps22df_interrupt_mode_get(&platform_spi_lps22df_dev, &int_mode);
    if (ret != 0)
    {
        return -1;
    }
    int_mode.active_low = PROPERTY_ENABLE;
    int_mode.drdy_latched = PROPERTY_DISABLE;
    ret = lps22df_interrupt_mode_set(&platform_spi_lps22df_dev, &int_mode);
    if (ret != 0)
    {
        return -1;
    }

    ret = lps22df_pin_int_route_get(&platform_spi_lps22df_dev, &int_route);
    if (ret != 0)
    {
        return -1;
    }
    int_route.drdy_pres = PROPERTY_ENABLE;
    ret = lps22df_pin_int_route_set(&platform_spi_lps22df_dev, &int_route);
    if (ret != 0)
    {
        return -1;
    }

    g_baro_drdy_flag = 0U;
    calibration_start_tick = HAL_GetTick();

    return 0;
}

/**
* @brief 设置气压基准动态跟踪使能状态
* @param enable 1表示静止，允许计时后动态校零；0表示运动，立即冻结基准
* @return 无
*/
void LPS27HHTW_BaselineTrackingSet(uint8_t enable)
{
    uint8_t tracking_enable = (enable != 0U) ? 1U : 0U;

    if ((tracking_enable != 0U) && (baseline_tracking_enable == 0U))
    {
        baseline_stationary_start_tick = HAL_GetTick();
        baseline_stationary_timing = 1U;
    }
    else if (tracking_enable == 0U)
    {
        baseline_stationary_start_tick = 0U;
        baseline_stationary_timing = 0U;
    }

    baseline_tracking_enable = tracking_enable;
}

/**
* @brief LPS27HHTW数据处理任务
* @param dT_ms 任务周期
* @return 无
*/
void LPS27HHTW_Task(uint16_t dT_ms)
{
    lps22df_all_sources_t all_sources = {0};
    lps22df_data_t data = {0};
    float raw_altitude_m;

    (void)dT_ms;

    if (g_baro_drdy_flag == 0U)
    {
        return;
    }
    g_baro_drdy_flag = 0U;

    if (lps22df_all_sources_get(&platform_spi_lps22df_dev, &all_sources) != 0)
    {
        return;
    }

    if ((all_sources.drdy_pres == 0U) && (all_sources.drdy_temp == 0U))
    {
        return;
    }

    if (lps22df_data_get(&platform_spi_lps22df_dev, &data) != 0)
    {
        return;
    }

    if (all_sources.drdy_pres != 0U)
    {
        data_raw_pressure = data.pressure.raw;
        pressure_hPa = data.pressure.hpa;

        if (is_calibrated == 0U)
        {
            altitude_m = 0.0f;
            barometer_altitude_ready = 0U;

            if ((HAL_GetTick() - calibration_start_tick) >= CALIBRATION_WAIT_MS)
            {
                pressure_sum += pressure_hPa;
                init_count++;

                if (init_count >= CALIBRATION_SAMPLES)
                {
                    reference_pressure_hPa = pressure_sum / (float)CALIBRATION_SAMPLES;
                    is_calibrated = 1U;
                }
            }
        }
        else
        {
            if ((baseline_tracking_enable != 0U) &&
                (baseline_stationary_timing != 0U) &&
                ((HAL_GetTick() - baseline_stationary_start_tick) >= BASELINE_STATIONARY_DELAY_MS))
            {
                reference_pressure_hPa += BASELINE_TRACKING_ALPHA *
                    (pressure_hPa - reference_pressure_hPa);
            }

            if ((reference_pressure_hPa > 0.0f) && (pressure_hPa > 0.0f))
            {
                raw_altitude_m = 44330.0f * (1.0f - powf(pressure_hPa / reference_pressure_hPa, 0.190295f));
            }
            else
            {
                raw_altitude_m = 0.0f;
            }

            altitude_m = altitude_average_filter_update(raw_altitude_m);
            barometer_altitude_ready = 1U;
        }
    }

    if (all_sources.drdy_temp != 0U)
    {
        data_raw_temperature = data.heat.raw;
        temperature_degC_LPS27 = data.heat.deg_c;
    }

    User_Barometer_Data_Update();

}
