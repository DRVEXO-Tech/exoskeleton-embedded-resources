#ifndef _DRVEXO_IMU_H_
#define _DRVEXO_IMU_H_

#include "main.h"
#include "DRVEXOIC.h"

/*参考坐标
俯视，正前方为x正方向
     +x
     |
 +y--|--
     |
*/	


typedef struct {
    float IMU_ACC_g[3];//单位重力加速度g,不是mg
    float IMU_GRYO_dps[3];//单位度每秒
    float Temperature;
    float dTs;
}IMU_Data_st;

typedef enum
{
    IMU1=0,
    IMU2,
    
    IMU_Num,
}IMU_Num_en;
extern IMU_Data_st  IMU_Data[IMU_Num];
extern IMU_Data_st  IMU_Data_IEM[IMU_Num];
extern DRVEXOAhrs ahrs;
extern DRVEXOEuler euler;
void IMU_Data_Init(void);
void IMU_Update(IMU_Data_st *Clean_IMU_Data);
void IMU_Updata_Task(uint16_t dT);
#endif
