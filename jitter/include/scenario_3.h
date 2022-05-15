#ifndef JITTER_SCENARIO_3_H
#define JITTER_SCENARIO_3_H

#include "scenario.h"

/* TASK 1 PARAMS */
#define TASK_1_PRIORITY_SCENARIO_3          0
/* TASK 2 PARAMS */
#define TASK_2_PRIORITY_SCENARIO_3          0
/* PERIODIC TASK */
#define PERIODIC_TASK_PRIORITY_SCENARIO_3   1

/* NEED TO ENABLE TIME SLICING IN ZEPHYR*/

void startJitterTestScenario_3()
{
    addSecondPeriodicTask(PERIODIC_TASK_PRIORITY_SCENARIO_3);
    startJitterTestScenario(TASK_1_PRIORITY_SCENARIO_3 , TASK_2_PRIORITY_SCENARIO_3 , PERIODIC_TASK_PRIORITY_SCENARIO_3, "scenario_3");
}

#endif // JITTER_SCENARIO_3_H