#include "semaphore_shuffle.h"

/* Board management API */
#include "board_config.h"

/* RTOS API */
#include "rtos_portable.h"

/* INTERFACE */
#include "benchmark_interface.h"

/* TASK 1 PARAMS */
#define TASK_1_PRIORITY   0

/* TASK 2 PARAMS */
#define TASK_2_PRIORITY   0

static struct TestResults testResults = {
    .testTime = 0,
    .testIteration = TEST_ITERATION
};

static void taskSemaphoreShuffleTest_1(void *pvParameters);
static void taskSemaphoreShuffleTest_2(void *pvParameters);

void startSemaphoreShuffleTest()
{
    if (!createSemaphore())
    {
        print("Error! Cannot create semaphore!\n");
        return;
    }
    createTask(taskSemaphoreShuffleTest_1, "SemaphoreShuffleTestTask_1", TASK_1_PRIORITY, TASK_1_INDEX);
}

void printSemaphoreShuffleTestResults()
{
    print("Semaphore shuffle test: testTime:%u\n", testResults.testTime);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

static void taskSemaphoreShuffleTest_1(void *pvParameters)
{
    createTask(taskSemaphoreShuffleTest_2, "SemaphoreShuffleTestTask_2", TASK_2_PRIORITY, TASK_2_INDEX);

    for (int i = 0; i < TEST_ITERATION; i++)
    {
        if (takeSemaphore())
        {
            switchTask();
            giveSemaphore();
            switchTask();
        }
    }

    switchTask();

    for (;;) {}
}

static void taskSemaphoreShuffleTest_2(void *pvParameters)
{
    for (int i = 0; i < TEST_ITERATION; i++)
    {
        startTimer();
        if (takeSemaphore())
        {
            testResults.testTime += getTimerValue();
            stopTimer();
            giveSemaphore();
            switchTask();
        }
    }

    sendResults(&testResults.testTime, 1);
    
    for (;;) {}
}
#pragma GCC pop_options