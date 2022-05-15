#include "task_switch.h"

/* Board management API */
#include "board_config.h"

/* RTOS API */
#include "rtos_portable.h"

/* INTERFACE */
#include "benchmark_interface.h"

/* TASK PARAMS */
#define TASK_PRIORITY   0

static uint32_t loopCmdTime = 0;
static struct TestResults testResults = {
    .testTime = 0,
    .testIteration = TEST_ITERATION
};

static uint8_t checkFlag = 0;

static void initTest();
static void taskSwitchTest_1(void *pvParameters);
static void taskSwitchTest_2(void *pvParameters);

void startTaskSwitchTest()
{
    createTask(taskSwitchTest_1, "TaskSwitchTest_1", TASK_PRIORITY, TASK_1_INDEX);
}

void printTestSwitchResults()
{
    print("Task switch test: testTime:%u\n", testResults.testTime);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
static void initTest()
{
    startTimer();
    for (uint32_t i = 0; i < TEST_ITERATION; i++)
    {
        ; /* JUST LOOP */
    }
    loopCmdTime = getTimerValue();
    stopTimer();
}

static void taskSwitchTest_1(void *pvParameters)
{
    initTest();

    createTask(taskSwitchTest_2, "TaskSwitchTest_2", TASK_PRIORITY, TASK_2_INDEX);

    for (uint32_t i = 0; i < TEST_ITERATION; i++)
    {
        startTimer();
        switchTask();
        if (!checkFlag)
        {
            print("Error! Task 2 does not work!\n");
            return;
        }
        checkFlag = 0;
    }

    testResults.testTime -= loopCmdTime;
    // printTestSwitchResults();
    sendResults(&testResults.testTime, 1);

    for (;;) {}
}

static void taskSwitchTest_2(void *pvParameters)
{
    for (uint32_t i = 0; i < TEST_ITERATION; i++)
    {
        testResults.testTime += getTimerValue();
        stopTimer();
        checkFlag = 1;
        switchTask();
    }

    for (;;) {}
}
#pragma GCC pop_options