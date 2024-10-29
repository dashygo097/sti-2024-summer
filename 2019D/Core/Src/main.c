/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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
#include "screen.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

ALIGN_32BYTES (uint16_t adc1_data[FO_LENGTH_LOW*2]) 	__attribute__((section(".ARM.__at_0x30000000")));
__IO uint8_t AdcConvEnd_1 = 0;

double v[FO_LENGTH_LOW*2 + 100];
double Pin[FO_LENGTH_LOW + 100];
double Pout[FO_LENGTH_LOW + 100];
double uin[FO_LENGTH_LOW + 100];
double uout[FO_LENGTH_LOW + 100];
double voltage = 0;
char str_2[50];
int max_index[2] = {0,0};
double max[2] = {-1 , -1};
double s1;
double *s = &s1;
double *p = Pin;
double *X;
int flag = 0;

char c ;

double Us,Ui,Uo1,Uo2,Us_DC,Ui_DC,Uo1_DC,Uo2_DC;
double Rin , Rout , Av , wave[250];

void adc_init(void)
{
//	HAL_UART_Transmit(&huart1,(uint8_t *)"start_adc\n",sizeof("start_adc\n"),HAL_MAX_DELAY);
	MX_ADC1_Init();
	HAL_Delay(1);
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)"hadc1 error with HAL_ADCEx_Calibration_Start\r\n",
				sizeof("hadc1 error with HAL_ADCEx_Calibration_Start\r\n"),HAL_MAX_DELAY);
			Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_data, FO_LENGTH_LOW*2) != HAL_OK)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)"hadc1 error with HAL_ADC_Start_DMA\r\n",
				sizeof("hadc1 error with HAL_ADC_Start_DMA\r\n"),HAL_MAX_DELAY);
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim15);
	while (!AdcConvEnd_1);
	AdcConvEnd_1 = 0;
	HAL_ADC_DeInit(&hadc1);
	HAL_TIM_Base_Stop(&htim15);
}



void Para_init(void)
{
	for (int i = 0 ; i < 2 ; i ++)
		  {
			  max[i] = 0;
			  max_index[i] = 0;

		  }
		  for ( int i = 0 ; i < FO_LENGTH_LOW ; i++)
		  {
			  Pin[i] = 0;
			  uin[i] = 0;
		  }
}


void ADC1_Get(void)
{

		adc_init();
		//v[0] =  ((double)adc1_data[0]*3.3)/65535 *3;
		  for (uint16_t temp = 0; temp< FO_LENGTH_LOW*2;temp++)
		  	    {
		  	    	  voltage = ((double)adc1_data[temp]*3.3)/65535 *3 ;
		  	    	  v[temp] = voltage;
		  	    	  if (temp%2==0){uin[temp/2] = voltage;}
		  	    	  else {uout[temp/2] = voltage;}
//		  	    	  sprintf(str_2 , "%.5f" , v[temp]);
//		  	    	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		  	    	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
		  	    }
		  for (uint16_t temp = 0; temp< FO_LENGTH_LOW;temp++)
		  		  		  	    {
		  		  		  	    	  sprintf(str_2 , "%.5f" , uin[temp]);
		  		  		  	    	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
		  		  		  	    	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
		  		  		  	    }
		  for (uint16_t temp = 0; temp< FO_LENGTH_LOW;temp++)
		  		  	    {
		  		  	    	  sprintf(str_2 , "%.5f" , uout[temp]);
		  		  	    	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
		  		  	    	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
		  		  	    }
		  //LowPassFilter(v);



}

void HandleFFT(void)
{
	s = FFT_ADC1(FO_LENGTH_LOW , uin ) ;
			  max_index[1] = (*s) * 106;
			  s1 = *s;
	for (int i = 0 ; i < 5 ; i ++)
	{
		if (flag == 1)
		{
			X= Blackman(FO_LENGTH_LOW ,(max_index[1]-max_index[1]/2),(max_index[1]+max_index[1]/2), uin);
		}
		else
		{
			X = uin;
		}


		p = FFT_Mag(FO_LENGTH_LOW , X);
		for (int j = 0 ; j  <FO_LENGTH_LOW ; j++)
		{
			Pin[j]+=p[j];
		}
	}

}

void PrintMag(void)
{
	 for (int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
	 {
//	 	sprintf(str_2 , "%.5lf" , (Pin[i]));
		if (max[0] < Pin[i] &&  i < 20 && i >=0 )
		{
		  	max_index[0] = i * 1 ;
		  	max[0] =Pin[i];

		}
		else if (i < 50 )
		{
		  	max_index[0] = max_index[0];
		  	max[0] = max[0];
		}
		if (max[1] < Pin[i] && i < (max_index[1]+20) &&  i >(max_index[1]-20) )
		{
		  	max_index[1] = i * 1 ;
		  	max[1] =Pin[i];

		}
//		HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
	 }
	 sprintf(str_2 , "%.2f" , s1);
	 HAL_UART_Transmit(&huart1 ,(uint8_t *)"Frequency:", 10 , HAL_MAX_DELAY);
	 HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
	 HAL_UART_Transmit(&huart1 ,(uint8_t *)"kHz\n", 4 , HAL_MAX_DELAY);
	 sprintf(str_2 , "%.5f" , sqrt(max[0]) / s1  / 94.1 );
	 HAL_UART_Transmit(&huart1 ,(uint8_t *)"Uo0:", 4 , HAL_MAX_DELAY);
	 HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
	 HAL_UART_Transmit(&huart1 ,(uint8_t *)"V\n", 2 , HAL_MAX_DELAY);
	 sprintf(str_2 , "%.5f" , sqrt(max[1])/ s1 / 23.5 /sqrt(2) /  2);
	 HAL_UART_Transmit(&huart1 ,(uint8_t *)"Uo1:", 4 , HAL_MAX_DELAY);
	 HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
	 HAL_UART_Transmit(&huart1 ,(uint8_t *)"V\n", 2 , HAL_MAX_DELAY);


}

void ADC_FFT(void)
{
	flag = 1;
	Para_init();
	ADC1_Get();
	HandleFFT();
	PrintMag();
	//HAL_Delay(500);UART_Screen
}



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM15_Init();
  MX_DAC1_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  AD9833_WaveSetting(1000 , 0 , SIN_WAVE , 0 , 0);
  ADC1_Get();
  Get_Ui();
  //Ui = FastGet_Ui();
  //Ui = FastGet_Ui();
  //Ui = FastGet_Ui(); ADC1_Get();
//  ADC1_Get();
////  	  ADC1_Get();
//  	  Get_Us();
//  	  //Us = FastGet_Us();
//  	//Us = FastGet_Us();
//  	  ADC1_Get();
//  	Get_Uo2();
//  	ADC1_Get();
//  	Get_Uo1();
//  	//Us = FastGet_Us();
////  	  ADC1_Get();
////  	    	  ADC1_Get();
//  	  Rin = Get_Rin();
//  	  Rout = Get_Rout();
//  	  Av = Get_Av();
//  		  for (int i = 0 ; i <250 ; i++)
//  		  {
//  				 wave[i] = i *i*0.001;
//  		  }
//  	  Screen_Show(Rin , Rout ,Av ,wave );
  //ADC1_Get();
  //adc_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	 sprintf(str_2 , "%.5f" , Rin);
		 HAL_UART_Transmit(&huart1 ,(uint8_t *)"Rin:", 4 , HAL_MAX_DELAY);
		 HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
		 HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 2 , HAL_MAX_DELAY);
		 sprintf(str_2 , "%.5f" , Rout);
		 HAL_UART_Transmit(&huart1 ,(uint8_t *)"Rout:", 5 , HAL_MAX_DELAY);
		 HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
		 HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 2 , HAL_MAX_DELAY);
		 sprintf(str_2 , "%.5f" , Av);
		 HAL_UART_Transmit(&huart1 ,(uint8_t *)"Av:", 3 , HAL_MAX_DELAY);
		 HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
		 HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 2 , HAL_MAX_DELAY);

		 		 HAL_Delay(10000);
//		 		Screen_Show(Rin , Rout ,Av ,wave );
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   if(hadc->Instance == ADC1)
	{
      //SCB_InvalidateDCache_by_Addr((uint32_t *) &adc1_data[0], ADC1_BUFFER_SIZE);
   }
	 AdcConvEnd_1 = 1;

}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART1)
    {
        if(c=='0')
        {
            USART1PutString("cle 8,0\xff\xff\xff");
        }
        if(c=='1')
        {
            fARelationship();
        }
     HAL_UART_Receive_IT(&huart1, (uint8_t*)&c,1);
    }
}


/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30020000;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
