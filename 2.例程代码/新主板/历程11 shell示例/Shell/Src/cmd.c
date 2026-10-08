#include "main.h"
#include "shell.h"
#include "APP_Control.h"
#include "DRVEXO_Scheduler.h"
#include <string.h>

#define CMD_MODE_TIMEOUT_MS 100U


uint8_t receive_flag =0;


void reboot(void)
{
		NVIC_SystemReset(); // Reset the microcontroller
}

SHELL_EXPORT_CMD(reboot, reboot, reboot the mcu);

int plus(int a, int b)
{
		return a + b;
}

SHELL_EXPORT_CMD(plus, plus, plus);

void ls(void)
{
    TaskTCB_t *task = pxTaskListHead;

    printf("Name             Priority  Runtime(us)\r\n");
    while (task != NULL)
    {
        if (task->pxTaskCode != NULL)
        {
            printf("%-16s %-9u %lu\r\n",
                   task->pcTaskName,
                   (unsigned int)task->ucPriority,
                   (unsigned long)task->Task_running_time_us);
        }
        task = task->pxNext;
    }
}

SHELL_EXPORT_CMD(ls, ls, View scheduler list);

int mode(uint32_t mode_index)
{
    uint32_t protocol_mode;
    const char *mode_name;
    int32_t ret;

    switch (mode_index)
    {
    case 0U:
        protocol_mode = 5U;
        mode_name = "rest";
        break;
    case 1U:
        protocol_mode = 0U;
        mode_name = "comfort";
        break;
    case 2U:
        protocol_mode = 2U;
        mode_name = "sports";
        break;
    case 3U:
        protocol_mode = 4U;
        mode_name = "damping";
        break;
    default:
        printf("Usage: mode <0|1|2|3>\r\n");
        return -1;
    }

    ret = APP_Control_SetBleSystemMode(protocol_mode, CMD_MODE_TIMEOUT_MS);
    if (ret == 0)
    {
        printf("Mode switched to %s\r\n", mode_name);
    }
    else
    {
        printf("Mode switch failed: %ld\r\n", (long)ret);
    }

    return ret;
}

SHELL_EXPORT_CMD_EX(mode, mode, switch system mode, mode <0|1|2|3>);

float a = 0.0f;

int set(char *name, uint32_t value_bits)
{
    float value;

    if (strcmp(name, "a") != 0)
    {
        printf("Variable '%s' not found\r\n", name);
        return -1;
    }

    memcpy(&value, &value_bits, sizeof(value));
    a = value;
    printf("a = %f\r\n", (double)a);
    return 0;
}

SHELL_EXPORT_CMD_EX(set, set, set float variable, set a <value>);

int var1 = 123;
SHELL_EXPORT_VAR_INT(var1, var1, var for test);



