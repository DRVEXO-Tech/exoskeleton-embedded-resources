/**
  ******************************************************************************
  * @file    DRVEXO_Scheduler.h
  * @brief   调度器头文件。
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
#ifndef _DRVEXO_SCHEDULER_H_
#define _DRVEXO_SCHEDULER_H_
#ifdef __cplusplus
extern "C" {
#endif
    
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/




//#define SHELL_ON

#define TASK_PRIO_MAX    31U
#define TASK_PRIO_HIGHEST 0U
#define TASK_PRIO_LOWEST  31U

#define MAX_STATIC_TASK_NUM     20U

#define TASK_STATE_FREE         0U  
#define TASK_STATE_USED_DYN     1U  
#define TASK_STATE_USED_STATIC  2U 

#define pdPASS 1
#define pdFAIL 0

/* Exported types ------------------------------------------------------------*/
typedef uint8_t BaseType_t;

typedef void (*TaskFunction_t)(uint16_t dT_ms);

typedef struct scheduler_task
{
    TaskFunction_t pxTaskCode;      
    char pcTaskName[16];            
    uint16_t usRateMs;              
    uint64_t ulLastRunTime;        
    uint8_t  ucPriority;            
    uint8_t  ucTaskState;           
    struct scheduler_task *pxNext; 
    uint32_t Task_running_time_us;  
} TaskTCB_t;

typedef TaskTCB_t* TaskHandle_t;

/* Exported extern variables -------------------------------------------------*/
extern TaskHandle_t IMUDataPollingHandle ;
extern TaskHandle_t IMUData2PollingHandle ;
extern TaskHandle_t IMUUpdataPollingHandle ;    
extern TaskHandle_t LEDHandle;
extern TaskHandle_t BarometerPollingHandle;
extern TaskHandle_t FDCANHandle ; 
extern TaskHandle_t VoioceHandle ;
extern TaskHandle_t LowPowerHandle;
extern TaskTCB_t *pxTaskListHead;
extern uint32_t cnt_time ;

#ifdef SHELL_ON
extern char ring_buf[512];
extern uint8_t uart_rx_buf[10];
#endif   
/* Exported functions prototypes ---------------------------------------------*/

BaseType_t Scheduler_CreateTask(TaskFunction_t pxTaskCode,
                                const char *pcName,
                                uint16_t usRateMs,
                                uint8_t ucPriority,
                                TaskHandle_t *pxCreatedTask);

BaseType_t Scheduler_CreateStaticTask(TaskFunction_t pxTaskCode,
                                      const char *pcName,
                                      uint16_t usRateMs,
                                      uint8_t ucPriority,
                                      TaskHandle_t *pxCreatedTask);

BaseType_t Scheduler_DeleteTask(TaskHandle_t xTaskToDelete);
uint32_t GetTick(void);
void Scheduler_List_Init(void);

void Scheduler_Init(void);

void Scheduler_Run(void);

#ifdef __cplusplus
}
#endif
#endif /* _DRVEXO_SCHEDULER_H_ */


