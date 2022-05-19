#ifndef JITTER_SCENARIO_4_H
#define JITTER_SCENARIO_4_H

#include "scenario.h"

/* APER TASK PARAMS */
#define APER_TASK_1_PRIORITY_SCENARIO_4         1
#define APER_TASK_REST_PRIORITY_SCENARIO_4      0

/* PERIODIC TASK */
#define PERIODIC_TASK_PRIORITY_SCENARIO_4       1

/* NEED TO ENABLE TIME SLICING IN ZEPHYR*/

void startJitterTestScenario_4()
{
    startJitterTestScenario(APER_TASK_1_PRIORITY_SCENARIO_4, APER_TASK_REST_PRIORITY_SCENARIO_4, PERIODIC_TASK_PRIORITY_SCENARIO_4, "scenario_4");
}

#endif // JITTER_SCENARIO_4_H
