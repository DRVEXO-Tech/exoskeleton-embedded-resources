#include "DRVEXO_Scheduler.h"
#include "APP_FDCAN.h"
#include "APP_ISM6DSO.h"
#include "APP_LPS27HHTW.h"
#include "APP_Voice.h"
#include "APP_LED.h"
#include "APP_LowPower.h"
#include "DRVEXO_IMU.h"
#include "Sys.h"
#include "shell.h"
#include "ring.h"

static TaskTCB_t xStaticTaskTCBPool[MAX_STATIC_TASK_NUM] = {0};
TaskTCB_t *pxTaskListHead = NULL;
uint32_t cnt_time =0;
uint32_t GetTick(void)
{
    return cnt_time;
}


static BaseType_t prvAddTaskToLinkedList(TaskTCB_t *pxNewTask)
{
    if (pxNewTask == NULL) return pdFAIL;

    if (pxTaskListHead == NULL)
    {
        pxTaskListHead = pxNewTask;
        pxNewTask->pxNext = NULL;
        return pdPASS;
    }

    if (pxNewTask->ucPriority < pxTaskListHead->ucPriority)
    {
        pxNewTask->pxNext = pxTaskListHead;
        pxTaskListHead = pxNewTask;
        return pdPASS;
    }

    TaskTCB_t *pxTemp = pxTaskListHead;

    while (pxTemp->pxNext != NULL)
    {
        if (pxNewTask->ucPriority < pxTemp->pxNext->ucPriority)
        {
            pxNewTask->pxNext = pxTemp->pxNext;
            pxTemp->pxNext = pxNewTask;
            return pdPASS;
        }
        pxTemp = pxTemp->pxNext;
    }

    pxTemp->pxNext = pxNewTask;
    pxNewTask->pxNext = NULL;
    return pdPASS;
}

static TaskTCB_t *prvGetFreeStaticTaskTCB(void)
{
    for (uint8_t i = 0; i < MAX_STATIC_TASK_NUM; i++)
    {
        if (xStaticTaskTCBPool[i].ucTaskState == TASK_STATE_FREE)
        {
            return &xStaticTaskTCBPool[i];
        }
    }
    return NULL; 
}



static void prvInitTaskTCB(TaskTCB_t *pxTask, TaskFunction_t pxTaskCode,
                           const char *pcName, uint16_t usRateMs,
                           uint8_t ucPriority, uint8_t ucTaskState)
{
    pxTask->pxTaskCode = pxTaskCode;
    pxTask->usRateMs = usRateMs;
    pxTask->ucPriority = ucPriority;
    pxTask->ulLastRunTime = GetTick();
    pxTask->Task_running_time_us = 0;
    pxTask->ucTaskState = ucTaskState;

    if (pcName != NULL && pcName[0] != '\0')
    {
        strncpy(pxTask->pcTaskName, pcName, 15);
        pxTask->pcTaskName[15] = '\0';
    }
    else
    {
        strcpy(pxTask->pcTaskName, "UnnamedTask");
    }
    pxTask->pxNext = NULL;
}


BaseType_t Scheduler_CreateTask(TaskFunction_t pxTaskCode,
                                const char *pcName,
                                uint16_t usRateMs,
                                uint8_t ucPriority,
                                TaskHandle_t *pxCreatedTask)
{

    if (pxTaskCode == NULL || usRateMs == 0 || ucPriority > TASK_PRIO_MAX)
    {
        return pdFAIL;
    }
    TaskTCB_t *pxNewTask = (TaskTCB_t *)malloc(sizeof(TaskTCB_t));
    if (pxNewTask == NULL)
    {
        return pdFAIL; 
    }

    prvInitTaskTCB(pxNewTask, pxTaskCode, pcName, usRateMs, ucPriority, TASK_STATE_USED_DYN);

    if (prvAddTaskToLinkedList(pxNewTask) != pdPASS)
    {
        free(pxNewTask); 
        return pdFAIL;
    }

    if (pxCreatedTask != NULL)
    {
        *pxCreatedTask = pxNewTask;
    }

    return pdPASS;

}

BaseType_t Scheduler_CreateStaticTask(TaskFunction_t pxTaskCode,
                                      const char *pcName,
                                      uint16_t usRateMs,
                                      uint8_t ucPriority,
                                      TaskHandle_t *pxCreatedTask)

{
    if (pxTaskCode == NULL || usRateMs == 0 || ucPriority > TASK_PRIO_MAX)
    {
        return pdFAIL;
    }
    TaskTCB_t *pxNewTask = prvGetFreeStaticTaskTCB();
    if (pxNewTask == NULL)
    {
        return pdFAIL; 
    }
    prvInitTaskTCB(pxNewTask, pxTaskCode, pcName, usRateMs, ucPriority, TASK_STATE_USED_STATIC);

    if (prvAddTaskToLinkedList(pxNewTask) != pdPASS)
    {
        pxNewTask->ucTaskState = TASK_STATE_FREE; 
        return pdFAIL;
    }

    if (pxCreatedTask != NULL)
    {
        *pxCreatedTask = pxNewTask;
    }

    return pdPASS;
}


BaseType_t Scheduler_DeleteTask(TaskHandle_t xTaskToDelete)
{
    if (xTaskToDelete == NULL || pxTaskListHead == NULL)
    {
        return pdFAIL;
    }
    TaskTCB_t *pxTemp = pxTaskListHead;
    TaskTCB_t *pxPrev = NULL;
    while (pxTemp != NULL)
    {
        if (pxTemp == xTaskToDelete)
        {
            if (pxPrev == NULL)
            {
                pxTaskListHead = pxTemp->pxNext;
            }
            else
            {
                pxPrev->pxNext = pxTemp->pxNext;
            }

            if (pxTemp->ucTaskState == TASK_STATE_USED_DYN)
            {
                free(pxTemp); 
            }
            else if (pxTemp->ucTaskState == TASK_STATE_USED_STATIC)
            {
                pxTemp->ucTaskState = TASK_STATE_FREE;
                pxTemp->pxTaskCode = NULL;
                memset(pxTemp->pcTaskName, 0, sizeof(pxTemp->pcTaskName));
            }
            pxTemp->pxNext = NULL; 
            return pdPASS;
        }
        pxPrev = pxTemp;
        pxTemp = pxTemp->pxNext;
    }

    return pdFAIL; 
}



void Scheduler_List_Init(void)
{
    pxTaskListHead = NULL;
    for (uint8_t i = 0; i < MAX_STATIC_TASK_NUM; i++)
    {
        memset(&xStaticTaskTCBPool[i], 0, sizeof(TaskTCB_t));
        xStaticTaskTCBPool[i].ucTaskState = TASK_STATE_FREE;
        xStaticTaskTCBPool[i].pxNext = NULL;
    }
}


#ifdef SHELL_ON
    ring uart_rx_ring;
    char ring_buf[512];
    SHELL_TypeDef shell;
    uint8_t uart_rx_buf[10];
    signed char myShellRead(char *c)
    {
        if (ring_poll(&uart_rx_ring, c) == 0)
            return 0;
        else
            return -1;
    }

    void myShellWrite(const char c)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)&c, 1, 1000);
    }

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
        if(huart->Instance == USART1)
        {
            ring_push(&uart_rx_ring, uart_rx_buf[0]);
            HAL_UART_Receive_IT(&huart1,uart_rx_buf,1);
        }
    }

    int fputc(int ch, FILE *f)
    {
        while ((USART1->ISR & USART_ISR_TXE_TXFNF) == 0);

        USART1->TDR = (uint8_t)ch;
        return ch;
    }

    void shell_run(uint16_t dT_ms)
    {
        shellTask(&shell);
    }
    TaskHandle_t SHELLPollingHandle = NULL;

#endif

    ///////////////////////////////////////

TaskHandle_t IMUDataPollingHandle = NULL;
TaskHandle_t IMUData2PollingHandle = NULL;
TaskHandle_t IMUUpdataPollingHandle = NULL;    
TaskHandle_t LEDHandle =NULL;
TaskHandle_t BarometerPollingHandle = NULL;
TaskHandle_t FDCANHandle = NULL; 
TaskHandle_t VoioceHandle = NULL;
TaskHandle_t LowPowerHandle = NULL;



void Scheduler_Init(void)
{
    Scheduler_List_Init();
#ifdef SHELL_ON
    shell.read = myShellRead;
	shell.write = myShellWrite;
    ring_init(&uart_rx_ring,ring_buf,512);
    shellInit(&shell);
    HAL_UART_Receive_IT(&huart1,uart_rx_buf,1);
    Scheduler_CreateTask(shell_run,   "SHELL",   10, 0, &SHELLPollingHandle);
#endif
     Scheduler_CreateStaticTask(IMU_Data_Task,   "IMUDATA",   10, 3, &IMUDataPollingHandle);
     if(Sub_IMU_Flag)
         Scheduler_CreateStaticTask(IMU_Data_Sub_Task,   "IMUDATA2",   10, 3, &IMUData2PollingHandle);
     Scheduler_CreateStaticTask(IMU_Updata_Task,   "IMUUpdata",   10, 4, &IMUUpdataPollingHandle);
     Scheduler_CreateStaticTask(LPS27HHTW_Task,   "Barometer",   1, 5, &BarometerPollingHandle);
     Scheduler_CreateStaticTask(APP_FDCAN_Task,   "FDCAN",   1, 21, &FDCANHandle);

     Scheduler_CreateStaticTask(APP_Voice_Task,   "Voice",   1, 21, &VoioceHandle);
     Scheduler_CreateStaticTask(APP_LED_TASK,   "LED",   1000, 21, &LEDHandle);
     if(Scheduler_CreateStaticTask(APP_LowPower_Task, "LowPower", 10, 22, &LowPowerHandle) != pdPASS)
     {
         sys_err.LowPower_Task_Create_err_cnt++;
     }
}
void Scheduler_Run(void)
{
    TaskTCB_t *pxTemp = pxTaskListHead;
    const uint32_t ulNow = GetTick();
    while (pxTemp != NULL)
    {
        uint32_t ulElapsedTime = ulNow - pxTemp->ulLastRunTime;
        if (ulElapsedTime >= pxTemp->usRateMs)
        {
            pxTemp->ulLastRunTime = ulNow; 
            if (pxTemp->pxTaskCode != NULL)
            {
                uint32_t Task_Running_Begin_Time = SysTick->VAL;
                 pxTemp->pxTaskCode(pxTemp->usRateMs);
                uint32_t Task_Running_Finish_Time = SysTick->VAL;
                if(Task_Running_Finish_Time >= Task_Running_Begin_Time)
                    Task_Running_Begin_Time +=250000;
                pxTemp->Task_running_time_us = (uint32_t)((float)(Task_Running_Begin_Time - Task_Running_Finish_Time)*0.004f);
            }
        }
        pxTemp = pxTemp->pxNext; 
    }
}
