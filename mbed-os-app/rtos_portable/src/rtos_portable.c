#include "rtos_portable.h"

void createTaskCpp(TaskFunction task, uint32_t priority, uint8_t taskIndex);
void switchTaskCpp();
void delayTaskCpp(uint32_t delay);
void suspendTaskCpp(uint8_t taskIndex);
void resumeTaskCpp(uint8_t taskIndex);
void resumeTaskFromIsrCpp(uint8_t taskIndex);
uint8_t createSemaphoreCpp();
uint8_t takeSemaphoreCpp();
uint8_t giveSemaphoreCpp();
uint8_t createQueueCpp(uint8_t queueSize);
uint8_t sendMsgCpp(void* msg);
uint8_t receiveMsgCpp(void* msg);
void createPeriodicTaskCpp(TaskFunction task, uint16_t taskPeriod, uint32_t priority, uint8_t taskIndex);
void deletePeriodicTaskCpp(uint8_t taskIndex);
void sleepTaskCpp(uint32_t time);

void createTask(TaskFunction task, const char *pcName, uint32_t priority, uint8_t taskIndex)
{
    (void) pcName;

    createTaskCpp(task, priority, taskIndex);				
}

void switchTask()
{
    switchTaskCpp();
}

void delayTask(uint32_t delay)
{
    delayTaskCpp(delay);
}

void suspendTask(uint8_t taskIndex)
{
    suspendTaskCpp(taskIndex);
}

void resumeTask(uint8_t taskIndex)
{
    resumeTaskCpp(taskIndex);
}

void resumeTaskFromIsr(uint8_t taskIndex)
{
    resumeTaskFromIsrCpp(taskIndex);
}

uint8_t createSemaphore()
{
    return createSemaphoreCpp();
}

uint8_t takeSemaphore()
{
    return takeSemaphoreCpp();
}

uint8_t giveSemaphore()
{
    return giveSemaphoreCpp();
}

uint8_t createQueue(uint8_t queueSize)
{
    return createQueueCpp(queueSize);
}

uint8_t sendMsg(void* msg)
{
    return sendMsgCpp(msg);
}

uint8_t receiveMsg(void* msg)
{
    return receiveMsgCpp(msg);
}

void createPeriodicTask(TaskFunction task, const char *pcName, uint16_t taskPeriod, uint32_t priority, uint8_t taskIndex)
{
    (void) pcName;
    createPeriodicTaskCpp(task, taskPeriod, priority, taskIndex);
}

void deletePeriodicTask(uint8_t taskIndex)
{
    deletePeriodicTaskCpp(taskIndex);
}

void sleepTask(uint32_t time)
{
    sleepTaskCpp(time);
}
