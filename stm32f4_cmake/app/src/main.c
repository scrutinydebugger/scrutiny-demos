//    main.c
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//   - Author : MrMati (Mateusz Niedbała) <mati.niedbala5@gmail.com>
//   - Contributors :
//       - Pier-Yves Lessard (pylessard)
//
//    Copyright (c) 2025 Scrutiny Debugger

#include "main.h"
#include "stm32f411e_discovery.h"
#include "stm32f411e_discovery_accelerometer.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "usb_device.h"

#include "scrutiny_integration.h"

I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void TIM2_Init(void);
static void TIM3_Init(void);

ACCELERO_XYZTypeDef accel_mgXYZ = { 0 };
static volatile struct
{
    GPIO_PinState B1_User; // PA0
    GPIO_PinState LD3;     // PD13
    GPIO_PinState LD4;     // PD12
    GPIO_PinState LD5;     // PD14
    GPIO_PinState LD6;     // PD15
} io;

void TIM3_IRQHandler(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_TIM_IRQHandler(&htim3);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t last_timestamp_us_tim3 = 0;
    if (htim->Instance == TIM3)
    {
        uint32_t const timestamp_us = TIM2->CNT;
        uint32_t const timediff_us = (timestamp_us - last_timestamp_us_tim3);
        last_timestamp_us_tim3 = timestamp_us;
        // We use a precise time reference to have an accurate "measured time". Should be a value very close to 1.000 ms
        scrutiny_c_loop_handler_fixed_freq_process(task_1khz_loop_handler, timediff_us * 10);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USB_DEVICE_Init();
    TIM2_Init();

    // Init before TIM3 since the timer invoke Scrutiny Loop Handler.
    if (scrutiny_integration_init() != 0)
    {
        Error_Handler();
    }

    if (BSP_ACCELERO_Init() != ACCELERO_OK)
    {
        Error_Handler();
    }

    TIM3_Init(); // 1KHz interrupt

    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
        uint32_t const timestamp_us = TIM2->CNT; // Runs at 1MHz

        // Passthrough of the user IOs for Scrutiny to control
        io.B1_User = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, io.LD3);
        HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, io.LD4);
        HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, io.LD5);
        HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, io.LD6);

        BSP_ACCELERO_GetXYZ(&accel_mgXYZ);         // blocking read. Takes ~3.17ms
        scrutiny_integration_update(timestamp_us); // Maximum refresh rate of 300 update/sec because of blocking accelerometer read
    }
}

void TIM2_Init(void)
{
    // Enable TIM2 time reference. CLK_INT = APB1 Timer = 48MHz
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 48 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xFFFFFFFF; // Free running to have a time reference
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
}

void TIM3_Init(void)
{
    // Enable TIM3 time reference. CLK_INT = APB1 Timer = 48MHz
    __HAL_RCC_TIM3_CLK_ENABLE();
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 48000 - 1;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : PA0 */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure GPIO pins : LD3_Pin LD4_Pin LD5_Pin LD6_Pin */
    GPIO_InitStruct.Pin = LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
       line) */
}
#endif /* USE_FULL_ASSERT */
