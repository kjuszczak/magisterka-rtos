#ifndef FREERTOS_HANDLERS_H
#define FREERTOS_HANDLERS_H

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
// #define xPortSysTickHandler SysTick_Handler

/* A header file that defines trace macro can be included here. */

void xPortSysTickHandler( void );

#endif /* FREERTOS_HANDLERS_H */