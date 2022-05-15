#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "board_config.h"

#include "test_selection.h"

#include "TCPExample.h"

#include "connection_time.h"

/**
* @brief    Task for blinking an LED every second
* 
* @param    pvParameters void pointer to task parameters
* 
* @retval   void
*/
void LedBlinky_Task(void *pvParameters) {
    for(;;) {
        // print("Hello world from FreeRTOS!\n");
        toggle_led();
        delay(1000);
    }
}

int main ( void )
{
    board_config();

    print("FreeRTOS\n");

    // startRtosTest();

    testMqtt();

    // xTaskCreate( LedBlinky_Task,						/* The function that implements the task. */
    //              "LedBlinky", 							/* The text name assigned to the task - for debug only as it is not used by the kernel. */
    //              configMINIMAL_STACK_SIZE, 				/* The size of the stack to allocate to the task. */
    //              NULL, 									/* The parameter passed to the task - just to check the functionality. */
    //              0, 										/* The priority assigned to the task. */
    //              NULL );									/* The task handle is not required, so NULL is passed. */

    vTaskStartScheduler();

    /* should never reach */
    for (;;) {}
    return 0;
}

void vLoggingPrintf( const char *pcFormat,
                     ... )
{
    /*In case of MQTT test comment this part*/
    // va_list val;
    // va_start(val, pcFormat);
    // uart_transmit(pcFormat, val);
    // va_end(val);
}
