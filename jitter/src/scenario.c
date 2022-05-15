#include "scenario.h"

/* Board management API */
#include "board_config.h"

/* RTOS API */
#include "rtos_portable.h"

/* INTERFACE */
#include "jitter_interface.h"

/* TASK 1*/
#define TASK_1_PERIOD       2 // ms

/* TASK 2 */
#define TASK_2_PERIOD       4 // ms

static void taskPeriodic(void *pvParameters);
static void taskJitter_1(void *pvParameters);
static void taskJitter_2(void *pvParameters);
static void taskPeriodic_2(void *pvParameters);

static struct TestResults results = {.testIteration = TEST_ITERATION};
static uint32_t loopIndex = 0;

static const char* testScenario;
static uint8_t taskPriority2 = 0;
static uint8_t perTaskPriority = 0;

void startJitterTestScenario(uint8_t taskPriority_1, uint8_t taskPriority_2, uint8_t periodicTaskPriority, const char* testScenarioName)
{
    testScenario = testScenarioName;
    taskPriority2 = taskPriority_2;
    perTaskPriority = periodicTaskPriority;

    createTask(taskJitter_1, "TaskJitter_1", taskPriority_1, TASK_1_INDEX);
}

void addSecondPeriodicTask(uint8_t periodicTaskPriority)
{
    createPeriodicTask(taskPeriodic_2, "TaskPeriodic_2", TASK_2_PERIOD, periodicTaskPriority, TASK_2_INDEX);
}

void printJitterTestResults()
{
    print("Jitter test %s: testTime:%u\n", testScenario, results.testTime[0]);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

static void taskPeriodic(void *pvParameters)
{
    results.testTime[loopIndex] = getTimerValue();

    if (loopIndex == TEST_ITERATION)
    {
        stopTimer();
        sendResults(results.testTime, TEST_ITERATION);
        deletePeriodicTask(0);
        deletePeriodicTask(1);
    }

    loopIndex++;
}

static void taskJitter_1(void *pvParameters)
{
    createTask(taskJitter_2, "TaskJitter_2", taskPriority2, TASK_2_INDEX);
    createPeriodicTask(taskPeriodic, "TaskPeriodic", TASK_1_PERIOD, perTaskPriority, TASK_1_INDEX);
    startTimer();

    for (;;) {}
}

static void taskJitter_2(void *pvParameters)
{
    for (;;) {}
}

static void taskPeriodic_2(void *pvParameters)
{
    for (uint16_t i = 0; i < 1000; i++) {}
}

#pragma GCC pop_options
