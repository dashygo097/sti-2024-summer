/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <string.h>
#include "signal.h"
#include "oled.h"
#include "FO.h"
#include <string.h>
#include "stm32h7xx.h"
#include "ad9833.h"
#include "measure.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

extern double uin[FO_LENGTH_LOW + 100];
extern double uout[FO_LENGTH_LOW + 100];
extern double v[FO_LENGTH_LOW*2 + 100];
extern ALIGN_32BYTES (uint16_t adc1_data[FO_LENGTH_LOW*2]) 	__attribute__((section(".ARM.__at_0x30000000")));
extern char str_2[50];
extern double Pin[FO_LENGTH_LOW + 100];
extern double Pout[FO_LENGTH_LOW + 100];
extern double voltage ;
extern double* p;

extern double Us,Ui,Uo1,Uo2,Us_DC,Ui_DC,Uo1_DC,Uo2_DC;


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void adc_init(void);
void ADC1_Get(void);
void Para_Init();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
