/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_SDI_Pin GPIO_PIN_1
#define ADC_SDI_GPIO_Port GPIOC
#define ADC_SDO_Pin GPIO_PIN_2
#define ADC_SDO_GPIO_Port GPIOC
#define SPI2_CS_Pin GPIO_PIN_3
#define SPI2_CS_GPIO_Port GPIOC
#define SafeADC3_Pin GPIO_PIN_0
#define SafeADC3_GPIO_Port GPIOA
#define SafeADC4_Pin GPIO_PIN_1
#define SafeADC4_GPIO_Port GPIOA
#define SafeADC2_Pin GPIO_PIN_2
#define SafeADC2_GPIO_Port GPIOA
#define BMS_CS_Pin GPIO_PIN_4
#define BMS_CS_GPIO_Port GPIOA
#define BMS_SCK_Pin GPIO_PIN_5
#define BMS_SCK_GPIO_Port GPIOA
#define BMS_MISO_Pin GPIO_PIN_6
#define BMS_MISO_GPIO_Port GPIOA
#define BMS_MOSI_Pin GPIO_PIN_7
#define BMS_MOSI_GPIO_Port GPIOA
#define SafeADC1_Pin GPIO_PIN_5
#define SafeADC1_GPIO_Port GPIOC
#define ADC_SCK_Pin GPIO_PIN_10
#define ADC_SCK_GPIO_Port GPIOB
#define CAN2_LBK_Pin GPIO_PIN_14
#define CAN2_LBK_GPIO_Port GPIOB
#define BMS_Ready_Pin GPIO_PIN_9
#define BMS_Ready_GPIO_Port GPIOC
#define User_LED_Pin GPIO_PIN_8
#define User_LED_GPIO_Port GPIOA
#define MCU_BSPD_Pin GPIO_PIN_10
#define MCU_BSPD_GPIO_Port GPIOC
#define MCU_PDOC_Pin GPIO_PIN_11
#define MCU_PDOC_GPIO_Port GPIOC
#define MCU_IMD_Pin GPIO_PIN_12
#define MCU_IMD_GPIO_Port GPIOC
#define MCU_AMS_Pin GPIO_PIN_2
#define MCU_AMS_GPIO_Port GPIOD
#define CAN1_LBK_Pin GPIO_PIN_7
#define CAN1_LBK_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
