#include "board_config.h"

#include "stm32f2xx_hal.h"

#define NANOPRINTF_IMPLEMENTATION
#include "nanoprintf.h"

#include <string.h>

/** @brief   LED pin */
#define LED1_PIN GPIO_PIN_7
/** @brief   LED port */
#define LED1_PORT GPIOB
/* END HAL */

/* LAN8742A_PHY_ADDRESS Address*/
#define LAN8742A_PHY_ADDRESS           0U

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

IsrRtosCallback tim1Callback;
GpioRtosCallback gpioCallback;

ETH_HandleTypeDef heth;

static char logBuffer[500] = "\r";

static void tim_2_config(void);
static void tim_5_config(void);

void board_config()
{
    HAL_Init();
    clock_config();
    uart_config();
    gpio_config();
    dma_config();
    tim_config();
    // eth_config();
}

void toggle_led()
{
    HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN);
}

void print(const char *pcFormat, ...)
{
    va_list val;
    va_start(val, pcFormat);
    uart_transmit(pcFormat, val);
    va_end(val);
}

uint8_t uart_transmit(const char *pcFormat, va_list args)
{
    npf_vsnprintf(logBuffer + 1, 249, pcFormat, args);
    return HAL_UART_Transmit(&huart3, (uint8_t*)logBuffer, strlen(logBuffer), HAL_MAX_DELAY) == HAL_OK;
    // return HAL_UART_Transmit_IT(&huart3, (uint8_t*)logBuffer, strlen(logBuffer)) == HAL_OK;
}

void delay(uint32_t period)
{
    HAL_Delay(period);
}

uint32_t getTimeTick()
{
    return HAL_GetTick();
}

void startTimer()
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_Base_Start(&htim2);
}

void stopTimer()
{
    HAL_TIM_Base_Stop(&htim2);
}

uint32_t getTimerValue()
{
    return __HAL_TIM_GET_COUNTER(&htim2);
}

void setGpioCallback(GpioRtosCallback rtosCallback)
{
    gpioCallback = rtosCallback;
}

void generateGpioInterrupt()
{
    __HAL_GPIO_EXTI_GENERATE_SWIT(GPIO_PIN_13);
}

void sendResults(uint32_t* testResults, uint16_t sizeOfResults)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)testResults, 4 * sizeOfResults, HAL_MAX_DELAY);
}

void resetSystem()
{
    NVIC_SystemReset();
}

/* PRIVATE FUNCTIONS */
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 240
  *            PLL_P                          = 2
  *            PLL_Q                          = 5
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
void clock_config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 240;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) ;
}

void gpio_config(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef  GPIOB_InitStruct = {0};
    GPIO_InitTypeDef  GPIOC_InitStruct = {0};

    GPIOB_InitStruct.Pin   = LED1_PIN;
    GPIOB_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIOB_InitStruct.Pull  = GPIO_PULLUP;
    GPIOB_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(LED1_PORT, &GPIOB_InitStruct);
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);

    GPIOC_InitStruct.Pin = GPIO_PIN_13;
    GPIOC_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIOC_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIOC_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void uart_config(void)
{
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart3);
}

/**
  * Enable DMA controller clock
  */
void dma_config(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
//   /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
}

void tim_config(void)
{
    tim_5_config();
    tim_2_config();
}

static void tim_5_config(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  HAL_TIM_Base_Init(&htim5);
  
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig);
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);
}

static void tim_2_config(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    tim1Callback();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    (void) GPIO_Pin;
    gpioCallback();
}

void eth_config(void)
{
   static uint8_t MACAddr[6];

  heth.Instance = ETH;
  heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  heth.Init.Speed = ETH_SPEED_100M;
  heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.RxMode = ETH_RXINTERRUPT_MODE;
  heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;

  HAL_ETH_Init(&heth);
}