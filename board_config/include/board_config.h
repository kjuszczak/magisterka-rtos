#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <stdarg.h>
#include <stdint.h>

#define TIMER_CLK 60000000

typedef void (* IsrRtosCallback) (void);
typedef void (* GpioRtosCallback) (void);

void board_config();
void print(const char *pcFormat, ...);
void toggle_led();
void delay(uint32_t period);

uint32_t getTimeTick();
void startTimer();
void stopTimer();
uint32_t getTimerValue();

void setGpioCallback(GpioRtosCallback rtosCallback);

void generateGpioInterrupt();

void sendResults(uint32_t* testResults, uint16_t sizeOfResults);

void resetSystem();

/* Private */
void clock_config(void);
void gpio_config(void);
void uart_config(void);
void dma_config(void);
void tim_config(void);
void eth_config(void);

uint8_t uart_transmit(const char *pcFormat, va_list args);

#endif // BOARD_CONFIG_H