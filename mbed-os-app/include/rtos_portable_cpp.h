#ifndef RTOS_PORTABLE_CPP_H
#define RTOS_PORTABLE_CPP_H

#include "stdint.h"

#define MAX_NUMBER_OF_TASKS             3
#define MAX_NUMBER_OF_PERIODIC_TASKS    2

typedef void (* TaskFunction) ();

extern "C" void createTaskCpp(TaskFunction task, uint32_t priority, uint8_t taskIndex);
extern "C" void switchTaskCpp();
extern "C" void delayTaskCpp(uint32_t delay);
extern "C" void suspendTaskCpp(uint8_t taskIndex);
extern "C" void resumeTaskCpp(uint8_t taskIndex);
extern "C" void resumeTaskFromIsrCpp(uint8_t taskIndex);
extern "C" uint8_t createSemaphoreCpp();
extern "C" uint8_t takeSemaphoreCpp();
extern "C" uint8_t giveSemaphoreCpp();
extern "C" uint8_t createQueueCpp(uint8_t queueSize);
extern "C" uint8_t sendMsgCpp(void* msg);
extern "C" uint8_t receiveMsgCpp(void* msg);
extern "C" void createPeriodicTaskCpp(TaskFunction task, uint16_t taskPeriod, uint32_t priority, uint8_t taskIndex);
extern "C" void deletePeriodicTaskCpp(uint8_t taskIndex);
extern "C" void sleepTaskCpp(uint32_t time);

#endif // RTOS_PORTABLE_CPP_H