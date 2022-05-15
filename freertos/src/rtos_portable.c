#include "rtos_portable.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#define TASK_BASE_PRIORITY 0u

static TaskHandle_t xHandleTask[MAX_NUMBER_OF_TASKS];

static SemaphoreHandle_t xSemaphore = NULL;
static QueueHandle_t xStructQueue = NULL;

static BaseType_t xYieldRequired;
static void vTimerCallback(TimerHandle_t timer);

static TimerHandle_t xTimer[MAX_NUMBER_OF_PERIODIC_TASKS];
static TaskFunction periodicTask[MAX_NUMBER_OF_PERIODIC_TASKS];
static TaskHandle_t periodicTaskHandler[MAX_NUMBER_OF_PERIODIC_TASKS];

void createTask(TaskFunction task, const char *pcName, uint32_t priority, uint8_t taskIndex)
{
    xTaskCreate(task,						
        pcName, 						
        configMINIMAL_STACK_SIZE, 			
        NULL, 								
        TASK_BASE_PRIORITY + priority, 								
        &xHandleTask[taskIndex]);	
}

void switchTask()
{
    taskYIELD();
}

void suspendTask(uint8_t taskIndex)
{
    vTaskSuspend(xHandleTask[taskIndex]);
}

void resumeTask(uint8_t taskIndex)
{
    vTaskResume(xHandleTask[taskIndex]);
}

void resumeTaskFromIsr(uint8_t taskIndex)
{
    BaseType_t xYieldRequired;

    xYieldRequired = xTaskResumeFromISR(xHandleTask[taskIndex]);
    portYIELD_FROM_ISR(xYieldRequired);
}

uint8_t createSemaphore()
{
    xSemaphore = xSemaphoreCreateMutex();
    return xSemaphore != NULL;
}

uint8_t takeSemaphore()
{
    return xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE;
}

uint8_t giveSemaphore()
{
    return xSemaphoreGive(xSemaphore) == pdTRUE;
}

uint8_t createQueue(uint8_t queueSize)
{
    xStructQueue = xQueueCreate(queueSize, sizeof(uint8_t));
    return xStructQueue != NULL;
}

uint8_t sendMsg(void* msg)
{
    return xQueueSend(xStructQueue, msg, portMAX_DELAY) == pdPASS;
}

uint8_t receiveMsg(void* msg)
{
    return xQueueReceive(xStructQueue, msg, portMAX_DELAY) == pdPASS;
}

static void vPeriodicTask_1(void* arg)
{
    for (;;)
    {
        vTaskSuspend(periodicTaskHandler[0]);
        periodicTask[0](NULL);
    }
}

static void vPeriodicTask_2(void* arg)
{
    for (;;)
    {
        vTaskSuspend(periodicTaskHandler[1]);
        periodicTask[1](NULL);
    }
}

static TaskFunction vPeriodicTasks[MAX_NUMBER_OF_PERIODIC_TASKS] = {vPeriodicTask_1, vPeriodicTask_2};

static void createTimer(const char *pcName, uint16_t taskPeriod, TimerHandle_t* timer)
{
     *timer = xTimerCreate
            ( /* Just a text name, not used by the RTOS
                kernel. */
                pcName,
                /* The timer period in ticks, must be
                greater than 0. */
                taskPeriod,
                /* The timers will auto-reload themselves
                when they expire. */
                pdTRUE,
                /* The ID is used to store a count of the
                number of times the timer has expired, which
                is initialised to 0. */
                ( void * ) 0,
                /* Each timer calls the same callback when
                it expires. */
                vTimerCallback
            );

    xTimerStart(*timer, 0);   
}

static void deleteTimer(TimerHandle_t timer)
{
    xTimerStop(timer, 0);
    xTimerDelete(timer, 0);
}

void createPeriodicTask(TaskFunction task, const char *pcName, uint16_t taskPeriod, uint32_t priority, uint8_t taskIndex)
{
    periodicTask[taskIndex] = task;

    xTaskCreate(vPeriodicTasks[taskIndex],						
        pcName, 						
        configMINIMAL_STACK_SIZE, 			
        NULL, 								
        TASK_BASE_PRIORITY + priority, 								
        &periodicTaskHandler[taskIndex]);	

    createTimer(pcName, taskPeriod, &xTimer[taskIndex]);
}

static void vTimerCallback(TimerHandle_t timer)
{
    if (timer == xTimer[0])
    {
        xYieldRequired = xTaskResumeFromISR(periodicTaskHandler[0]);
        portYIELD_FROM_ISR(xYieldRequired);
    }
    else
    {
        xYieldRequired = xTaskResumeFromISR(periodicTaskHandler[1]);
        portYIELD_FROM_ISR(xYieldRequired);
    }
}

void deletePeriodicTask(uint8_t taskIndex)
{
    deleteTimer(xTimer[taskIndex]);
}

void sleepTask(uint32_t time)
{
    vTaskDelay(time / portTICK_PERIOD_MS);
}
