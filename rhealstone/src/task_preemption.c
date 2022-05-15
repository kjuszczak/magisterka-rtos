#include "task_preemption.h"

/* Board management API */
#include "board_config.h"

/* RTOS API */
#include "rtos_portable.h"

/* INTERFACE */
#include "benchmark_interface.h"

/* TASK 1 PARAMS */
#define TASK_1_PRIORITY         1

/* TASK 2 PARAMS */
#define TASK_2_PRIORITY         0

static struct TestResults testResults = {
    .testTime = 0,
    .testIteration = TEST_ITERATION
};

static uint8_t checkFlag = 0;

static void isrCallback();
static void taskPreemptionTest_1(void *pvParameters);
static void taskPreemptionTest_2(void *pvParameters);

void startTaskPreemptionTest()
{
    createTask(taskPreemptionTest_1, "TaskPreemptionTest_1", TASK_1_PRIORITY, TASK_1_INDEX);
}

void printTestPreemptionResults()
{
    print("Task preemption test: testTime:%u\n", testResults.testTime);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

static void isrCallback()
{
    startTimer();
    resumeTaskFromIsr(TASK_1_INDEX);
}

static void taskPreemptionTest_1(void *pvParameters)
{
    setGpioCallback(isrCallback);

    createTask(taskPreemptionTest_2, "TaskPreemptionTest_2", TASK_2_PRIORITY, TASK_2_INDEX);

    for (int i = 0; i < TEST_ITERATION; i++)
    {
        checkFlag = 0;

        suspendTask(TASK_1_INDEX);
        testResults.testTime += getTimerValue();
        stopTimer();

        if (!checkFlag)
        {
            print("Error! Task 2 does not work\n");
            return;
        }
    }

    // printTestPreemptionResults();
    sendResults(&testResults.testTime, 1);

    for (;;) {}
}

static void taskPreemptionTest_2(void *pvParameters)
{
    for (int i = 0; i < TEST_ITERATION; i++)
    {   
        checkFlag = 1;
        generateGpioInterrupt();
    }

    for (;;) {}
}
#pragma GCC pop_options