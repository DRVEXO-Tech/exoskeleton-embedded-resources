#include "DRVEXO_IMU.h"
#include "APP_ISM6DSO.h"
#include "User_Data.h"
/*
    此IMU基于MadgwickAHRS算法
    梯度下降互补滤波
    具有速度快，计算少，同时效果优异
*/
#define SAMPLE_RATE (100) // replace with actual sample rate

IMU_Data_st  IMU_Data[IMU_Num];
IMU_Data_st  IMU_Data_IEM[IMU_Num];
float imu_dbg_earth_acc_bias_g[3] = {0.0f, 0.0f, 0.0f};
static uint16_t imu_earth_acc_bias_count = 0U;
static float imu_earth_acc_bias_sum_g[3] = {0.0f, 0.0f, 0.0f};

// 定义低通滤波系数，用于计算动态方差 (值越小，方差计算越平滑)
#define VAR_ALPHA 0.05f 

// 传感器固有底噪方差极小值 (防止方差为 0 导致除以 0 崩溃)
#define MIN_VARIANCE 0.001f

/*
    IEM 旋转矩阵
    
*/
const DRVEXOMatrix IEM = {1.0f, 0.0f, 0.0f, 
                          0.0f, -1.0f, 0.0f, 
                          0.0f, 0.0f, -1.0f};

const DRVEXOMatrix IEM_Sub = {  0.0f, -1.0f, 0.0f, 
                                0.0f, 0.0f, -1.0f, 
                                1.0f, 0.0f, 0.0f};  

uint8_t fast_gyro_calib_done[IMU_Num] = {0, 0}; 
/*
    gyroscopeSensitivity    陀螺仪计比例因子补偿 
    gyroscopeOffset         陀螺仪计零偏补偿
*/

DRVEXOVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};

DRVEXOVector gyroscopeOffset[IMU_Num] = {
    {0.0f, 0.0f, 0.0f}, // IMU1 默认零偏
    {0.0f, 0.0f, 0.0f}  // IMU2 默认零偏
};
DRVEXOBias bias[IMU_Num];
/*
    gyroscopeSensitivity    加速度计比例因子补偿 
    gyroscopeOffset         加速度计零偏补偿
*/
DRVEXOVector accelerometerSensitivity = {.axis={0.962f, 1.015f, 1.0f}};

DRVEXOVector accelerometerOffset = {.axis={-0.04f, -0.05f, 0.001f}};

DRVEXOAhrs ahrs;
DRVEXOEuler euler;//最终输出欧拉角
const DRVEXOAhrsSettings settings = {
    .convention = DRVEXOConventionNwu,
    .gain = 0.5f,               //算法参数
    .gyroscopeRange = 2000.0f, /* 陀螺仪量程 */
    .accelerationRejection = 10.0f,/*加速度抗干扰拒绝阈值*/
//    .magneticRejection = 10.0f,  /*磁场抗干扰拒绝阈值*///没用到
    .recoveryTriggerPeriod = 5 * SAMPLE_RATE, /*干扰恢复超时周期*/
};
void IMU_Data_Init(void)
{
    DRVEXOBiasInitialise(&bias[IMU1], SAMPLE_RATE);
    DRVEXOBiasInitialise(&bias[IMU2], SAMPLE_RATE);
    DRVEXOAhrsInitialise(&ahrs);   
    DRVEXOAhrsSetSettings(&ahrs, &settings);
}
void Rapid_Gyro_Calibration_Background(float *gyr_dps_in, float dT_s, uint8_t imu_id)
{
    if (fast_gyro_calib_done[imu_id]) return;
    static float gyr_lpf[2][3] = {0};
    static float hold_time_ms[2] = {0};
    static uint16_t sample_cnt[2] = {0};
    static float gyr_sum[2][3] = {0};
    static uint8_t init_flag[2] = {0};
    float alpha = 2.0f * 3.1415926f * 2.0f * dT_s; 
    if (alpha > 1.0f) alpha = 1.0f;
    float delta_sq_sum = 0.0f;
    for(int i = 0; i < 3; i++) {
        if (init_flag[imu_id] == 0) gyr_lpf[imu_id][i] = gyr_dps_in[i];
        gyr_lpf[imu_id][i] += alpha * (gyr_dps_in[i] - gyr_lpf[imu_id][i]);
        float delta = gyr_dps_in[i] - gyr_lpf[imu_id][i];
        delta_sq_sum += delta * delta;
    }
    init_flag[imu_id] = 1;
    float delta_length = sqrtf(delta_sq_sum);
    if (delta_length > 0.5f) { 
        hold_time_ms[imu_id] = 0.0f;
        sample_cnt[imu_id] = 0; 
        gyr_sum[imu_id][0] = gyr_sum[imu_id][1] = gyr_sum[imu_id][2] = 0.0f;
    } else {
        hold_time_ms[imu_id] += dT_s * 1000.0f; 
    }
    if (hold_time_ms[imu_id] >= 200.0f) {
        gyr_sum[imu_id][0] += gyr_dps_in[0];
        gyr_sum[imu_id][1] += gyr_dps_in[1];
        gyr_sum[imu_id][2] += gyr_dps_in[2];
        sample_cnt[imu_id]++;
        if (sample_cnt[imu_id] >= 100) {
            gyroscopeOffset[imu_id].axis.x = gyr_sum[imu_id][0] / sample_cnt[imu_id];
            gyroscopeOffset[imu_id].axis.y = gyr_sum[imu_id][1] / sample_cnt[imu_id];
            gyroscopeOffset[imu_id].axis.z = gyr_sum[imu_id][2] / sample_cnt[imu_id];
            fast_gyro_calib_done[imu_id] = 1; 
        }
    }
}
//实时动态方差估计
void Update_Recursive_Variance(float current_val, float *mean, float *variance)
{
    float diff = current_val - *mean;
    *mean += VAR_ALPHA * diff;
    *variance = (1.0f - VAR_ALPHA) * (*variance + VAR_ALPHA * diff * diff);
    if (*variance < MIN_VARIANCE) *variance = MIN_VARIANCE;
}
//卡尔曼最优方差融合
float Optimal_Fusion(float val1, float var1, float val2, float var2)
{
    float total_variance = var1 + var2;
    float weight1 = var2 / total_variance;
    float weight2 = var1 / total_variance;
    return (weight1 * val1) + (weight2 * val2);
}
//
void Dual_IMU_Kalman_Fusion_Aligned(float *aligned_g1, float *aligned_g2, float *fused_gyro)
{
    static float mean1[3] = {0}, var1[3] = {MIN_VARIANCE, MIN_VARIANCE, MIN_VARIANCE};
    static float mean2[3] = {0}, var2[3] = {MIN_VARIANCE, MIN_VARIANCE, MIN_VARIANCE};
    for(int i = 0; i < 3; i++) {
        Update_Recursive_Variance(aligned_g1[i], &mean1[i], &var1[i]);
        Update_Recursive_Variance(aligned_g2[i], &mean2[i], &var2[i]);
        fused_gyro[i] = Optimal_Fusion(aligned_g1[i], var1[i], aligned_g2[i], var2[i]);
    }
}

static void IMU_Data_Get(float *acc,float *gyro,float tem,float dTs)
{
    memcpy(IMU_Data[IMU1].IMU_ACC_g,acc,sizeof(IMU_Data[IMU1].IMU_ACC_g));
    memcpy(IMU_Data[IMU1].IMU_GRYO_dps,gyro,sizeof(IMU_Data[IMU1].IMU_GRYO_dps));
    IMU_Data[IMU1].Temperature = tem;
    IMU_Data[IMU1].dTs = dTs;
}

static void IMU_Data_Get_Sub(float *acc,float *gyro,float dTs)
{
    memcpy(IMU_Data[IMU2].IMU_ACC_g,acc,sizeof(IMU_Data[IMU2].IMU_ACC_g));
    memcpy(IMU_Data[IMU2].IMU_GRYO_dps,gyro,sizeof(IMU_Data[IMU2].IMU_GRYO_dps));
    IMU_Data[IMU2].dTs = dTs;
}

void IMU_PreProcess(uint8_t use_dual_imu)
{
    DRVEXOVector gyro, acc;
    
    
    //主IMU上电快速零飘校准
    Rapid_Gyro_Calibration_Background(IMU_Data[IMU1].IMU_GRYO_dps, IMU_Data[IMU1].dTs, IMU1);
    gyro.axis.x = IMU_Data[IMU1].IMU_GRYO_dps[0]; 
    gyro.axis.y = IMU_Data[IMU1].IMU_GRYO_dps[1]; 
    gyro.axis.z = IMU_Data[IMU1].IMU_GRYO_dps[2];
    acc.axis.x = IMU_Data[IMU1].IMU_ACC_g[0];     
    acc.axis.y = IMU_Data[IMU1].IMU_ACC_g[1];     
    acc.axis.z = IMU_Data[IMU1].IMU_ACC_g[2];
    //acc和gyro零飘，安装矩阵，偏差校准
    gyro = DRVEXOModelInertial(gyro, IEM, gyroscopeSensitivity, gyroscopeOffset[IMU1]);
    acc  = DRVEXOModelInertial(acc, IEM, accelerometerSensitivity, accelerometerOffset);
    //运动过程中一但静止，更新零飘
    gyro = DRVEXOBiasUpdate(&bias[IMU1], gyro);
    IMU_Data_IEM[IMU1].IMU_GRYO_dps[0] = gyro.axis.x; 
    IMU_Data_IEM[IMU1].IMU_GRYO_dps[1] = gyro.axis.y; 
    IMU_Data_IEM[IMU1].IMU_GRYO_dps[2] = gyro.axis.z;
    IMU_Data_IEM[IMU1].IMU_ACC_g[0] = acc.axis.x;     
    IMU_Data_IEM[IMU1].IMU_ACC_g[1] = acc.axis.y;     
    IMU_Data_IEM[IMU1].IMU_ACC_g[2] = acc.axis.z;
    IMU_Data_IEM[IMU1].dTs = IMU_Data[IMU1].dTs;
    if (use_dual_imu == 1) 
    {
        //次级IMU上电快速零飘更新
        Rapid_Gyro_Calibration_Background(IMU_Data[IMU2].IMU_GRYO_dps, IMU_Data[IMU2].dTs, IMU2);
        gyro.axis.x = IMU_Data[IMU2].IMU_GRYO_dps[0];
        gyro.axis.y = IMU_Data[IMU2].IMU_GRYO_dps[1];
        gyro.axis.z = IMU_Data[IMU2].IMU_GRYO_dps[2];
        //次级矩阵更新
        gyro = DRVEXOModelInertial(gyro, IEM_Sub, gyroscopeSensitivity, gyroscopeOffset[IMU2]);
        //运动过程中一但静止，更新零飘
        gyro = DRVEXOBiasUpdate(&bias[IMU2], gyro);
        IMU_Data_IEM[IMU2].IMU_GRYO_dps[0] = gyro.axis.x; 
        IMU_Data_IEM[IMU2].IMU_GRYO_dps[1] = gyro.axis.y; 
        IMU_Data_IEM[IMU2].IMU_GRYO_dps[2] = gyro.axis.z;
        float fused_gyro[3];
        //双IMU融合角速度
        Dual_IMU_Kalman_Fusion_Aligned(IMU_Data_IEM[IMU1].IMU_GRYO_dps, IMU_Data_IEM[IMU2].IMU_GRYO_dps, fused_gyro);
        IMU_Data_IEM[IMU1].IMU_GRYO_dps[0] = fused_gyro[0];
        IMU_Data_IEM[IMU1].IMU_GRYO_dps[1] = fused_gyro[1];
        IMU_Data_IEM[IMU1].IMU_GRYO_dps[2] = fused_gyro[2];
    }
}
//更新四元数
void IMU_Update(IMU_Data_st *IMU_Data)
{
    DRVEXOVector gyroscope = { .axis = { 
        IMU_Data[IMU1].IMU_GRYO_dps[0], 
        IMU_Data[IMU1].IMU_GRYO_dps[1], 
        IMU_Data[IMU1].IMU_GRYO_dps[2] 
    }};
    DRVEXOVector accelerometer = { .axis = { 
        IMU_Data[IMU1].IMU_ACC_g[0], 
        IMU_Data[IMU1].IMU_ACC_g[1], 
        IMU_Data[IMU1].IMU_ACC_g[2] 
    }};
    DRVEXOAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, IMU_Data[IMU1].dTs);
    euler = DRVEXOQuaternionToEuler(DRVEXOAhrsGetQuaternion(&ahrs));
}
void IMU_Updata_Task(uint16_t dT)
{
    IMU_Data_Get(ACC_g,GRYO_dps,Temperature,dTs);
    if(Sub_IMU_Flag)
        IMU_Data_Get_Sub(ACC_g_sub,GRYO_dps_sub,dTs_sub);
    IMU_PreProcess(Sub_IMU_Flag);
    IMU_Update(IMU_Data_IEM);
    User_IMU_Data_Update();
}

    
