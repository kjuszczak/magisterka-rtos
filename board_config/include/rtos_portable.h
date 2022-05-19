#ifndef RTOS_PORTABLE_H
#define RTOS_PORTABLE_H

#include "stdint.h"

#define MAX_NUMBER_OF_TASKS             5
#define MAX_NUMBER_OF_PERIODIC_TASKS    2
#define TASK_1_INDEX                    0
#define TASK_2_INDEX                    1
#define TASK_3_INDEX                    2
#define TASK_4_INDEX                    3
#define TASK_5_INDEX                    4

typedef void (* TaskFunction) (void*);

void createTask(TaskFunction task, const char *pcName, uint32_t priority, uint8_t taskIndex);
void switchTask();
void suspendTask(uint8_t taskIndex);
void resumeTask(uint8_t taskIndex);
void resumeTaskFromIsr(uint8_t taskIndex);
uint8_t createSemaphore();
uint8_t takeSemaphore();
uint8_t giveSemaphore();
uint8_t createQueue(uint8_t queueSize);
uint8_t sendMsg(void* msg);
uint8_t receiveMsg(void* msg);
void createPeriodicTask(TaskFunction task, const char *pcName, uint16_t taskPeriod, uint32_t priority, uint8_t taskIndex);
void deletePeriodicTask(uint8_t taskIndex);
void sleepTask(uint32_t time);
void disconnect();

#endif // RTOS_PORTABLE_H