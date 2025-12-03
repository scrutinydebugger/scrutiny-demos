/**
  ******************************************************************************
  * @file    stm32f411e_discovery.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage LEDs and
  *          push-button available on STM32F411-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f411e_discovery.h"

/** @defgroup BSP BSP
  * @{
  */ 

/** @defgroup STM32F411E_DISCOVERY STM32F411E discovery
  * @{
  */   
    
/** @defgroup STM32F411E_discovery_LOW_LEVEL STM32F411E discovery LOW LEVEL
  * @brief This file provides set of firmware functions to manage Leds and push-button
  *        available on STM32F411-Discovery Kit from STMicroelectronics.
  * @{
  */ 

/** @defgroup STM32F411E_discovery_LOW_LEVEL_Private_TypesDefinitions STM32F411E discovery LOW LEVEL Privat TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32F411E_discovery_LOW_LEVEL_Private_Defines STM32F411E discovery LOW LEVEL Private Defines
  * @{
  */ 
  
/**
  * @brief STM32F411E DISCO BSP Driver version number V1.0.5
  */
#define __STM32F411E_DISCO_BSP_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32F411E_DISCO_BSP_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32F411E_DISCO_BSP_VERSION_SUB2   (0x05) /*!< [15:8]  sub2 version */
#define __STM32F411E_DISCO_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __STM32F411E_DISCO_BSP_VERSION         ((__STM32F411E_DISCO_BSP_VERSION_MAIN << 24)\
                                             |(__STM32F411E_DISCO_BSP_VERSION_SUB1 << 16)\
                                             |(__STM32F411E_DISCO_BSP_VERSION_SUB2 << 8 )\
                                             |(__STM32F411E_DISCO_BSP_VERSION_RC))
/**
  * @}
  */ 

/** @defgroup STM32F411E_discovery_LOW_LEVEL_Private_Macros STM32F411E discovery LOW LEVEL Private Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32F411E_discovery_LOW_LEVEL_Private_Variables STM32F411E discovery LOW LEVEL Private Variables
  * @{
  */ 
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED4_GPIO_PORT, 
                                 LED3_GPIO_PORT, 
                                 LED5_GPIO_PORT,
                                 LED6_GPIO_PORT};

const uint16_t GPIO_PIN[LEDn] = {LED4_PIN, 
                                 LED3_PIN, 
                                 LED5_PIN,
                                 LED6_PIN};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {KEY_BUTTON_GPIO_PORT}; 
const uint16_t BUTTON_PIN[BUTTONn] = {KEY_BUTTON_PIN};
const uint8_t BUTTON_IRQn[BUTTONn] = {KEY_BUTTON_EXTI_IRQn};

uint32_t I2cxTimeout = I2Cx_TIMEOUT_MAX;    /*<! Value of Timeout when I2C communication fails */
uint32_t SpixTimeout = SPIx_TIMEOUT_MAX;    /*<! Value of Timeout when SPI communication fails */

static I2C_HandleTypeDef I2cHandle;
/**
  * @}
  */ 

/** @defgroup STM32F411E_discovery_LOW_LEVEL_Private_FunctionPrototypes STM32F411E discovery LOW LEVEL Private FunctionPrototypes
  * @{
  */
/* I2Cx bus function */
static void    I2Cx_Init(void);
static void    I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value);
static uint8_t I2Cx_ReadData(uint16_t Addr, uint8_t Reg);
static void    I2Cx_Error (void);
static void    I2Cx_MspInit(I2C_HandleTypeDef *hi2c);


/* Link function for GYRO peripheral */
void GYRO_IO_Init(void);
void GYRO_IO_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void GYRO_IO_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

/* Link functions for AUDIO */
void    AUDIO_IO_Init(void);
void    AUDIO_IO_DeInit(void);
void    AUDIO_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t AUDIO_IO_Read(uint8_t Addr, uint8_t Reg);

/* Link function for COMPASS / ACCELERO peripheral */
void    COMPASSACCELERO_IO_Init(void);
void    COMPASSACCELERO_IO_ITConfig(void);
void    COMPASSACCELERO_IO_Write(uint16_t DeviceAddr, uint8_t RegisterAddr, uint8_t Value);
uint8_t COMPASSACCELERO_IO_Read(uint16_t DeviceAddr, uint8_t RegisterAddr);
/**
  * @}
  */ 

/** @defgroup STM32F411E_discovery_LOW_LEVEL_Private_Functions STM32F411E discovery LOW LEVEL Private Functions
  * @{
  */ 
  
/**
  * @brief  This method returns the STM32F411 DISCO BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion(void)
{
  return __STM32F411E_DISCO_BSP_VERSION;
}

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured. 
  *   This parameter can be one of following parameters:
  *     @arg LED4
  *     @arg LED3
  *     @arg LED5
  *     @arg LED6
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Enable the GPIO_LED Clock */
  LEDx_GPIO_CLK_ENABLE(Led);

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = GPIO_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  
  HAL_GPIO_Init(GPIO_PORT[Led], &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET); 
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED4
  *     @arg LED3
  *     @arg LED5
  *     @arg LED6  
  */
void BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED4
  *     @arg LED3
  *     @arg LED5
  *     @arg LED6 
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET); 
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg LED4
  *     @arg LED3
  *     @arg LED5
  *     @arg LED6  
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter should be: BUTTON_KEY
  * @param  ButtonMode: Specifies Button mode.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                            generation capability  
  */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* Enable the BUTTON Clock */
  BUTTONx_GPIO_CLK_ENABLE(Button);
  
  if(ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    GPIO_InitStruct.Pin = BUTTON_PIN[Button];
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);
  }
  
  if(ButtonMode == BUTTON_MODE_EXTI)
  {
    /* Configure Button pin as input with External interrupt */
    GPIO_InitStruct.Pin = BUTTON_PIN[Button];
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; 
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);
    
    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
    HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  }
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter should be: BUTTON_KEY  
  * @retval The Button GPIO pin value.
  */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/*******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/

/******************************* I2C Routines *********************************/

/**
  * @brief  I2Cx Bus initialization.
  */
static void I2Cx_Init(void)
{
  if(HAL_I2C_GetState(&I2cHandle) == HAL_I2C_STATE_RESET)
  {
    I2cHandle.Instance = DISCOVERY_I2Cx;
    I2cHandle.Init.OwnAddress1 =  0x43;
    I2cHandle.Init.ClockSpeed = I2Cx_MAX_COMMUNICATION_FREQ;
    I2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    I2cHandle.Init.OwnAddress2 = 0x00;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;	

    /* Init the I2C */
    I2Cx_MspInit(&I2cHandle);
    HAL_I2C_Init(&I2cHandle);
  }
}

/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  Value: The target register value to be written 
  */
static void I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2cxTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    I2Cx_Error();
  }
}

/**
  * @brief  Reads a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @retval Data read at register address
  */
static uint8_t I2Cx_ReadData(uint16_t Addr, uint8_t Reg)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  
  status = HAL_I2C_Mem_Read(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    I2Cx_Error();
  }
  return value;
}

/**
  * @brief  I2Cx error treatment function.
  */
static void I2Cx_Error(void)
{
  /* De-initialize the I2C comunication BUS */
  HAL_I2C_DeInit(&I2cHandle);
  
  /* Re- Initiaize the I2C comunication BUS */
  I2Cx_Init();
}

/**
  * @brief  I2Cx MSP Init.
  * @param  hi2c: I2C handle
  */
static void I2Cx_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable the I2C peripheral */
  DISCOVERY_I2Cx_CLOCK_ENABLE();

  /* Enable SCK and SDA GPIO clocks */
  DISCOVERY_I2Cx_GPIO_CLK_ENABLE();

  /* I2Cx SD1 & SCK pin configuration */
  GPIO_InitStructure.Pin = DISCOVERY_I2Cx_SDA_PIN | DISCOVERY_I2Cx_SCL_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Alternate = DISCOVERY_I2Cx_AF;
  
  HAL_GPIO_Init(DISCOVERY_I2Cx_GPIO_PORT, &GPIO_InitStructure);

  /* Force the I2C peripheral clock reset */
  DISCOVERY_I2Cx_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  DISCOVERY_I2Cx_RELEASE_RESET();

  /* Enable and set I2Cx Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(DISCOVERY_I2Cx_EV_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(DISCOVERY_I2Cx_EV_IRQn);

  /* Enable and set I2Cx Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(DISCOVERY_I2Cx_ER_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(DISCOVERY_I2Cx_ER_IRQn); 
}




/********************************* LINK AUDIO *********************************/

/**
  * @brief  Initializes Audio low level.
  */
void AUDIO_IO_Init(void) 
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Enable Reset GPIO Clock */
  AUDIO_RESET_GPIO_CLK_ENABLE();
  
  /* Audio reset pin configuration -------------------------------------------*/
  GPIO_InitStruct.Pin = AUDIO_RESET_PIN; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(AUDIO_RESET_GPIO, &GPIO_InitStruct);
  
  I2Cx_Init();
  
  /* Power Down the codec */
  CODEC_AUDIO_POWER_OFF();
  
  /* Wait for a delay to insure registers erasing */
  HAL_Delay(5); 
  
  /* Power on the codec */
  CODEC_AUDIO_POWER_ON();
  
  /* Wait for a delay to insure registers erasing */
  HAL_Delay(5); 
}

/**
  * @brief  DeInitializes Audio low level.
  */
void AUDIO_IO_DeInit(void) 
{
  
}

/**
  * @brief  Writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address 
  * @param  Value: Data to be written
  */
void AUDIO_IO_Write (uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  I2Cx_WriteData(Addr, Reg, Value);
}

/**
  * @brief  Reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address 
  * @retval Data to be read
  */
uint8_t AUDIO_IO_Read (uint8_t Addr, uint8_t Reg)
{
  return I2Cx_ReadData(Addr, Reg);
}

/****************************** LINK ACCELEROMETER ****************************/

/**
  * @brief  Configures COMPASS / ACCELERO I2C interface.
  */
void COMPASSACCELERO_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable DRDY clock */
  ACCELERO_DRDY_GPIO_CLK_ENABLE();
  
  /* MEMS DRDY pin configuration */
  GPIO_InitStructure.Pin = ACCELERO_DRDY_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull  = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(ACCELERO_DRDY_GPIO_PORT, &GPIO_InitStructure);
  
  I2Cx_Init();
}

/**
  * @brief  Configures COMPASS / ACCELERO click IT.
  */
void COMPASSACCELERO_IO_ITConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable INT1 and INT2 GPIO clock */
  ACCELERO_INT_GPIO_CLK_ENABLE();
  
  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructure.Pin = ACCELERO_INT1_PIN | ACCELERO_INT2_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(ACCELERO_INT_GPIO_PORT, &GPIO_InitStructure);
  
  /* Enable and set COMPASS / ACCELERO Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(ACCELERO_INT1_EXTI_IRQn, 0x0F, 0x00);
  HAL_NVIC_EnableIRQ(ACCELERO_INT1_EXTI_IRQn);
}

/**
  * @brief  Writes one byte to the COMPASS / ACCELERO.
  * @param  DeviceAddr: the slave address to be programmed
  * @param  RegisterAddr: the COMPASS / ACCELERO register to be written
  * @param  Value: Data to be written
 */
void COMPASSACCELERO_IO_Write(uint16_t DeviceAddr, uint8_t RegisterAddr, uint8_t Value)
{
  /* Call I2Cx Read data bus function */
  I2Cx_WriteData(DeviceAddr, RegisterAddr, Value);
}

/**
  * @brief  Reads a block of data from the COMPASS / ACCELERO.
  * @param  DeviceAddr: the slave address to be programmed(ACC_I2C_ADDRESS or MAG_I2C_ADDRESS).
  * @param  RegisterAddr: the COMPASS / ACCELERO internal address register to read from
  * @retval COMPASS / ACCELERO register value
  */
uint8_t COMPASSACCELERO_IO_Read(uint16_t DeviceAddr, uint8_t RegisterAddr)
{
  /* Call I2Cx Read data bus function */   
  return I2Cx_ReadData(DeviceAddr, RegisterAddr);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */   
