#include "deadlock_breaking_time.h"

/* Board management API */
#include "board_config.h"

/* RTOS API */
#include "rtos_portable.h"

/* INTERFACE */
#include "benchmark_interface.h"

/* TASK 1 PARAMS */
#define TASK_1_PRIORITY   2

/* TASK 2 PARAMS */
#define TASK_2_PRIORITY   1

/* TASK 3 PARAMS */
#define TASK_3_PRIORITY   0

static struct TestResults testResults = {
    .testTime = 0,
    .testIteration = TEST_ITERATION
};

static void taskDeadlockBreakingTimeTest_1(void *pvParameters);
static void taskDeadlockBreakingTimeTest_2(void *pvParameters);
static void taskDeadlockBreakingTimeTest_3(void *pvParameters);

void startDeadlockBreakingTimeTest()
{
    if (!createSemaphore())
    {
        print("Error! Cannot create semaphore!\n");
        return;
    }
    createTask(taskDeadlockBreakingTimeTest_1, "DeadlockBreakingTimeTestTask_1", TASK_1_PRIORITY, TASK_1_INDEX);
    // createTask(taskDeadlockBreakingTimeTest_2, "DeadlockBreakingTimeTestTask_2", TASK_2_PRIORITY, TASK_2_INDEX);
    createTask(taskDeadlockBreakingTimeTest_3, "DeadlockBreakingTimeTestTask_3", TASK_3_PRIORITY, TASK_3_INDEX);
}

void printDeadlockBreakingTimeTestResults()
{
    print("Deadlock breaking test: testTime:%u\n", testResults.testTime);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

static void taskDeadlockBreakingTimeTest_1(void *pvParameters)
{
    for (int i = 0; i < TEST_ITERATION; i++)
    {
        suspendTask(TASK_1_INDEX);
        startTimer();
        if (takeSemaphore())
        {
            testResults.testTime += getTimerValue();
            stopTimer();
            giveSemaphore();
        }
    }

    // printDeadlockBreakingTimeTestResults();
    sendResults(&testResults.testTime, 1);

    for (;;) {}
}

static void taskDeadlockBreakingTimeTest_2(void *pvParameters)
{
    for (int i = 0; i < TEST_ITERATION; i++)
    {
        suspendTask(TASK_2_INDEX);
        resumeTask(TASK_1_INDEX);
    }
    for (;;) {}
}

static void taskDeadlockBreakingTimeTest_3(void *pvParameters)
{
    for (int i = 0; i < TEST_ITERATION; i++)
    {
        if (takeSemaphore())
        {
            resumeTask(TASK_1_INDEX);
        }
        giveSemaphore();
    }

    for (;;) {}
}
#pragma GCC pop_options