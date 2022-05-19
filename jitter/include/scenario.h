#ifndef JITTER_SCENARIO_H
#define JITTER_SCENARIO_H

#include "stdint.h"

void startJitterTestScenario(uint8_t task1Priority, uint8_t restTasksPriority, uint8_t periodicTaskPriority, const char* testScenarioName);
void addSecondPeriodicTask(uint8_t periodicTaskPriority);
void printJitterTestResults();

#endif // JITTER_SCENARIO_H
