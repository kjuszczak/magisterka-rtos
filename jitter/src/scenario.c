#include "scenario.h"

/* Board management API */
#include "board_config.h"

/* RTOS API */
#include "rtos_portable.h"

/* INTERFACE */
#include "jitter_interface.h"

/* PERIODICAL TASKS PARAMETERS */
#define TASK_1_PERIOD               2 // ms
#define TASK_2_PERIOD               4 // ms
#define TASK_1_FOR_ITERATION        10000

/* APER TASK PARAMETERS */      
#define APER_TASK_1_DELAY           5 // ms
#define APER_TASK_2_DELAY           6 // ms
#define APER_TASK_3_DELAY           7 // ms
#define APER_TASK_4_DELAY           8 // ms
#define APER_TASK_5_DELAY           9 // ms

#define APER_TASK_1_FOR_ITERATION   10000
#define APER_TASK_2_FOR_ITERATION   20000
#define APER_TASK_3_FOR_ITERATION   30000
#define APER_TASK_4_FOR_ITERATION   40000
#define APER_TASK_5_FOR_ITERATION   50000


static void taskPeriodic(void *pvParameters);
static void taskJitter_1(void *pvParameters);
static void taskJitter_2(void *pvParameters);
static void taskJitter_3(void *pvParameters);
static void taskJitter_4(void *pvParameters);
static void taskJitter_5(void *pvParameters);
static void taskPeriodic_2(void *pvParameters);

static struct TestResults results = {.testIteration = TEST_ITERATION};
static uint32_t loopIndex = 0;

static const char* testScenario;
static uint8_t aperTaskPriority = 0;
static uint8_t perTaskPriority = 0;

void startJitterTestScenario(uint8_t task1Priority, uint8_t restTasksPriority, uint8_t periodicTaskPriority, const char* testScenarioName)
{
    testScenario = testScenarioName;
    aperTaskPriority = restTasksPriority;
    perTaskPriority = periodicTaskPriority;

    createTask(taskJitter_1, "TaskJitter_1", task1Priority, TASK_1_INDEX);
}

void addSecondPeriodicTask(uint8_t periodicTaskPriority)
{
    createPeriodicTask(taskPeriodic_2, "TaskPeriodic_2", TASK_2_PERIOD, periodicTaskPriority, TASK_2_INDEX);
}

void printJitterTestResults()
{
    print("Jitter test %s: testTime_0:%u, testTime_1:%u, testTime_2:%u, testTime_3:%u, testTime_4:%u, testTime_5:%u, testTime_6:%u\n",
        testScenario,
        results.testTime[0],
        results.testTime[1],
        results.testTime[2],
        results.testTime[3],
        results.testTime[4],
        results.testTime[5],
        results.testTime[6]);
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
        // printJitterTestResults();
        deletePeriodicTask(0);
        deletePeriodicTask(1);
    }

    loopIndex++;
}

static void taskJitter_1(void *pvParameters)
{
    createTask(taskJitter_2, "TaskJitter_2", aperTaskPriority, TASK_2_INDEX);
    createTask(taskJitter_3, "TaskJitter_3", aperTaskPriority, TASK_3_INDEX);
    createTask(taskJitter_4, "TaskJitter_4", aperTaskPriority, TASK_4_INDEX);
    createTask(taskJitter_5, "TaskJitter_5", aperTaskPriority, TASK_5_INDEX);
    createPeriodicTask(taskPeriodic, "TaskPeriodic", TASK_1_PERIOD, perTaskPriority, TASK_1_INDEX);
    startTimer();

    for (;;)
    {
        for (uint16_t i = 0; i < APER_TASK_1_FOR_ITERATION; i++) { ; }
        sleepTask(APER_TASK_1_DELAY);
    }
}

static void taskJitter_2(void *pvParameters)
{
    for (;;)
    {
        for (uint16_t i = 0; i < APER_TASK_2_FOR_ITERATION; i++) { ; }
        sleepTask(APER_TASK_2_DELAY);
    }
}

static void taskJitter_3(void *pvParameters)
{
    for (;;)
    {
        for (uint16_t i = 0; i < APER_TASK_3_FOR_ITERATION; i++) { ; }
        sleepTask(APER_TASK_3_DELAY);
    }
}

static void taskJitter_4(void *pvParameters)
{
    for (;;)
    {
        for (uint16_t i = 0; i < APER_TASK_4_FOR_ITERATION; i++) { ; }
        sleepTask(APER_TASK_4_DELAY);
    }
}

static void taskJitter_5(void *pvParameters)
{
    for (;;)
    {
        for (uint16_t i = 0; i < APER_TASK_5_FOR_ITERATION; i++) { ; }
        sleepTask(APER_TASK_5_DELAY);
    }
}

static void taskPeriodic_2(void *pvParameters)
{
    for (uint16_t i = 0; i < TASK_1_FOR_ITERATION; i++) { ; }
}

#pragma GCC pop_options
