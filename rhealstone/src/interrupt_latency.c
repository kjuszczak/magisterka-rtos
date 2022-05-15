#include "interrupt_latency.h"

/* Board management API */
#include "board_config.h"

/* RTOS API */
#include "rtos_portable.h"

/* INTERFACE */
#include "benchmark_interface.h"

/* TASK 1 PARAMS */
#define TASK_1_PRIORITY   1

/* TASK 2 PARAMS */
#define TASK_2_PRIORITY   0

static struct TestResults testResults = {
    .testTime = 0,
    .testIteration = TEST_ITERATION
};

static void initTest();
static void isrCallback();
static void taskInterruptLatencyTest_1(void *pvParameters);
static void taskInterruptLatencyTest_2(void *pvParameters);

void startInterruptLatencyTest()
{
    createTask(taskInterruptLatencyTest_1, "InterruptLatencyTestTask_1", TASK_1_PRIORITY, TASK_1_INDEX);
}

void printInterruptLatencyTestResults()
{
    print("Interrupt latency test: testTime:%u\n", testResults.testTime);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
static void initTest()
{
    setGpioCallback(isrCallback);
}

static void isrCallback()
{
    testResults.testTime += getTimerValue();
}

static void taskInterruptLatencyTest_1(void *pvParameters)
{
    createTask(taskInterruptLatencyTest_2, "InterruptLatencyTestTask_2", TASK_2_PRIORITY, TASK_2_INDEX);

    initTest();

    for (int i = 0; i < TEST_ITERATION; i++)
    {
        startTimer();
        generateGpioInterrupt();
        stopTimer();
    }

    // printInterruptLatencyTestResults();
    sendResults(&testResults.testTime, 1);

    for (;;) {}
}

static void taskInterruptLatencyTest_2(void *pvParameters)
{
    for (;;)
    {
        delay(10); // delay 10 ms
    }
}
#pragma GCC pop_options