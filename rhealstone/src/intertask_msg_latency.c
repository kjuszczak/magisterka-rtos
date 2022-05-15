#include "intertask_msg_latency.h"

/* Board management API */
#include "board_config.h"

/* RTOS API */
#include "rtos_portable.h"

/* INTERFACE */
#include "benchmark_interface.h"

#define QUEUE_SIZE        10

/* TASK 1 PARAMS */
#define TASK_1_PRIORITY   0
#define TASK_1_DEFAULT_VAL_MSG 10

/* TASK 2 PARAMS */
#define TASK_2_PRIORITY   0
#define TASK_2_DEFAULT_VAL_MSG 20

static struct TestResults testResults = {
    .testTime = 0,
    .testIteration = TEST_ITERATION
};

static uint8_t msgTask_1 = TASK_1_DEFAULT_VAL_MSG;
static uint8_t msgTask_2 = TASK_2_DEFAULT_VAL_MSG;

static void taskIntertaskMsgLatencyTest_1(void *pvParameters);
static void taskIntertaskMsgLatencyTest_2(void *pvParameters);

void startIntertaskMsgLatencyTest()
{
    if (!createQueue(QUEUE_SIZE))
    {
        print("Error! Cannot create queue!\n");
        return;
    }

    createTask(taskIntertaskMsgLatencyTest_1, "IntertaskMsgLatencyTestTask_1", TASK_1_PRIORITY, TASK_1_INDEX);
}

void printIntertaskMsgLatencyTestResults()
{
    print("Intertask message latency test: testTime:%u\n", testResults.testTime);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

static void taskIntertaskMsgLatencyTest_1(void *pvParameters)
{
    createTask(taskIntertaskMsgLatencyTest_2, "IntertaskMsgLatencyTestTask_2", TASK_2_PRIORITY, TASK_2_INDEX);

    for (int i = 0; i < TEST_ITERATION; i++)
    {
        switchTask();
        msgTask_1 = 1;
        startTimer();
        if (sendMsg((void*)&msgTask_1))
        {
            switchTask();
        }
    }

    // printIntertaskMsgLatencyTestResults();
    sendResults(&testResults.testTime, 1);

    for (;;) {}
}

static void taskIntertaskMsgLatencyTest_2(void *pvParameters)
{
    for (int i = 0; i < TEST_ITERATION; i++)
    {
        if (receiveMsg((void*)&msgTask_2))
        {
            testResults.testTime += getTimerValue();
            stopTimer();
            if (msgTask_1 != msgTask_2)
            {
                print("Error! Wrong msg!\n");
                return;
            }
            msgTask_1 = TASK_1_DEFAULT_VAL_MSG;
            msgTask_2 = TASK_2_DEFAULT_VAL_MSG;
            switchTask();
        }
    }

    for (;;) {}
}
#pragma GCC pop_options