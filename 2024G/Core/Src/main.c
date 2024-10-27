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
#include <math.h>
#include "SSM.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

ALIGN_32BYTES (uint16_t adc1_data[FO_LENGTH_HIGH]) 	__attribute__((section(".ARM.__at_0x30000000")));
ALIGN_32BYTES (uint16_t adc1_music_data[FO_LENGTH_MIDDLE]) 	__attribute__((section(".ARM.__at_0x30080000")));

__IO uint8_t AdcConvEnd = 0;

int max_index[2] = {0 ,0}  ,min_index[3] = {0,0 , 0};
double max[5] = {-1,-1 ,-1 ,-1 ,-1 };
char str_2[50];
double * X ;
double *Y;
double P[FO_LENGTH_HIGH*4 +100];
double *p;
double voltage;
double v[FO_LENGTH_HIGH + 100];
double SP[FO_LENGTH_HIGH*8 + 100];
int i = 0;
int flag = 0;
int count =0 ;
int music = 0;
int state = 0 ;
double Spec[60] ;
int hold_on  =0;
double rate = 0;
double aaa;

char c = '9';

int hold_music = 0;
int hold_voice = 0;
int hold_weak = 0;
int human_killed = 0;
int music_killed = 0;
int f = 0;
int kill_human = 0;
int kill_music = 0;

uint8_t CNT=0;


void PWM_Output(int frequency , double r)
{
	rate = r;
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	//WRITE_REG(TIM4->CCR1,(TIM4->ARR+1)*rate);
	__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,(TIM4->ARR+1)*rate);
	TIM4->ARR=1000000/frequency-1;
	TIM4->CR1 &= ~TIM_CR1_ARPE;
}


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

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_data, FO_LENGTH_HIGH) != HAL_OK)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)"hadc1 error with HAL_ADC_Start_DMA\r\n",
				sizeof("hadc1 error with HAL_ADC_Start_DMA\r\n"),HAL_MAX_DELAY);
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim15);
	while (!AdcConvEnd);
	AdcConvEnd = 0;
	HAL_ADC_DeInit(&hadc1);
	HAL_TIM_Base_Stop(&htim15);
}


void adc_music_init(void)
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

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_music_data, FO_LENGTH_MIDDLE) != HAL_OK)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)"hadc1 error with HAL_ADC_Start_DMA\r\n",
				sizeof("hadc1 error with HAL_ADC_Start_DMA\r\n"),HAL_MAX_DELAY);
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim15);
	while (!AdcConvEnd);
	AdcConvEnd = 0;
	HAL_ADC_DeInit(&hadc1);
	HAL_TIM_Base_Stop(&htim15);
}



void Para_Init(void)
{

	 hold_music = 0;
	 hold_voice = 0;
	 hold_weak = 0;
//	 human_killed = 0;
//	 music_killed = 0;

	for (i = 0 ; i < 4 ; i ++)
		  {
			  max[i] = 0;
			  max_index[i] = 0;

		  }
	for ( i = 0 ; i < FO_LENGTH_HIGH*2 ; i++)
		  {
				  P[i] = 0;
				 // p[i] = 0;
		  }
		  count = 0;
		  hold_on=0;
}

void ADC1_Get(void)
{

		adc_init();

		  for (uint16_t temp = 0; temp< FO_LENGTH_HIGH;temp++)
		  	    {
		  	    	  voltage = ((double)adc1_data[temp]*3.3)/65535 *3 ;

		  	    	  v[temp] = voltage ;
//		  	    	if (fabs(v[temp] - 4.95) > max[4])
//		  	    	{
//		  	    		max[4] =v[temp] - 4.95;
//		  	    	}
		  	    	 // v[temp ] = 0;
//		  	    	  if (fabs(v[temp] - 4.7) > 2.0 )
//		  	    	  {
//		  	    		  sprintf(str_2 , "%.5f" , v[temp]);
//		  	    		  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		  	    		  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
//		  	    		  count++;
//		  	    	  }
		  	    }
}



void ADC1_Get_Music(void)
{
	  __HAL_TIM_SET_AUTORELOAD(&htim15, 500 -1);
	  TIM15->ARR = 500 - 1;
	  TIM15->CR1 &= ~TIM_CR1_ARPE;
		adc_music_init();

		  for (uint16_t temp = 0; temp< FO_LENGTH_MIDDLE;temp++)
		  	    {
		  	    	  voltage = ((double)adc1_music_data[temp]*3.3)/65535 *3 ;

		  	    	  SP[temp] = voltage - 4.95;
//		  	    	  SP[temp*2 + 1] = 0.0 - voltage;
		  	    	 // v[temp ] = 0;x
//		  	    	  if (fabs(v[temp] - 4.7) > 2.0 )
//		  	    	  {
//		  	    		  sprintf(str_2 , "%.5f" , SP[temp]);
//		  	    		  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		  	    		  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
//		  	    		  count++;
//		  	    	  }
		  	    }
//		  for (uint16_t temp = 0; temp< 400;temp++)
//		  {
//			  SP[temp + FO_LENGTH_MIDDLE]  = 0;
//		  }
}


void Input_Simulate(void)
{
	for (int k = 0 ; k < 256 ; k++)
	{
		for (i =0; i < 16 ; i++)
		{
			v[i+32*k] = sin(2*M_PI*i/16) + 1.5;
		}
		for (i =0; i < 16 ; i++)
		{
			v[i+32*k + 16] = sin(2*M_PI*i/16) + 1.5;
		}
	}
	  for (uint16_t temp = 0; temp< FO_LENGTH_HIGH;temp++)
	  	    {
//		  	    sprintf(str_2 , "%.5f" , v[temp]);
//		  	    HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		  	    HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
	  	    }
}


void HandleFFT(void)
{
			  	  for (i = 1 ; i <= 1; i++)
			  	  {
			  		  if (flag == 1)
			  		  {
			  			X= Blackman(FO_LENGTH_HIGH ,(max_index[1]-max_index[1]/2),(max_index[1]+max_index[1]/2), v);
			  		  }
			  		  else
			  		  {
			  			  X = v;
			  		  }

			  		  p = FFT_Mag(FO_LENGTH_HIGH , X);
			  		  for (int j = 0 ; j  <FO_LENGTH_HIGH/2 ; j++)
			  		  {
			  			  P[j]=p[j] * 5;
			  		  }
			  	  }
}

void PrintMag(void)
{
	 for (i = 0 ; i < FO_LENGTH_HIGH/2 ; i++)
	 {

//		 sprintf(str_2 , "%.5lf" , (P[i]));
//		 HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		 HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
		 if (i > 10 && i < 1000)
		 {
			 if ( (P[i] > 2 ||hold_on >=1 )  )
		 	 {
				 HAL_GPIO_WritePin(GPIOA , GPIO_PIN_1 , GPIO_PIN_RESET);
				 hold_on = 1;
		 	 }
			 else if (hold_on ==0)
			 {
				 hold_on  =0 ;
				 HAL_GPIO_WritePin(GPIOA , GPIO_PIN_1 , GPIO_PIN_SET);
			 }
		 }
	 }
	 max[4] = 0;
	 if (hold_on)USART1PutString("t4.txt=\"Get Sound!\"\xff\xff\xff");
	 else {USART1PutString("t4.txt=\"Weak Sound!\"\xff\xff\xff");}
}

void ASK_Output(int fre)
{
	setFre(fre);
	HAL_TIM_Base_Start_IT(&htim5);
}

void Music_Init(void)
{
	i = 0;
	for (double j = 0 ; j < 60 ; j+=1)
	{
		Spec[i] =261.63 * pow(2.0 ,j/12);
					i++;
	}
	__HAL_TIM_SET_AUTORELOAD(&htim15, 1000-1 );
		TIM15->ARR =  1000-1;
		htim5.Init.Period = 1000-1;
		TIM15->CR1 &= ~TIM_CR1_ARPE;
	i= 0 ;
	max[0] = 0;
	max[1] = 0;
	max[2] = 0;
}

void Is_Music(void)
{
	int c = 0;
		Para_Init();
		for (int j = 0 ; j < 8; j++)
		{
			ADC1_Get();
			for (int k = 0 ; k < FO_LENGTH_HIGH; k++)
			{
				SP[j*FO_LENGTH_HIGH + k] = v[k];
			}

		}
		double step = 1198.0/1024.0;
		for (int w = 0 ; w < 30 ; w+=2)
		{
			X=Hamming(FO_LENGTH_HIGH*4 ,(Spec[w]-Spec[w]/2)/step,(Spec[w]+Spec[w]/2)/step ,0.468 ,SP);
			p = FFT_Mag(FO_LENGTH_HIGH*4 , X);
			for (i = 0 ; i < FO_LENGTH_HIGH*2; i++)
			{
				P[i] += p[i];
			}
		}
		for (int l = 0 ; l < FO_LENGTH_HIGH*2 ; l++)
		{
			if (max[1] < P[l] && l > 30 && l < 2000)
			{
				max[1] = P[l];
				max_index[1] = l;
			}
			sprintf(str_2 , "%.5lf" , (P[l]));
			HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
		}
		for (int w = 0 ; w < 30 ; w++)
		{
			int index = (int)(Spec[w]/step + 0.5);
			double max_reg = -1;
			for (int dex = index -1 ; dex < index+2 ;dex++)
			{
				if (P[dex] > max_reg)
				{
					max_reg = P[dex];
				}
			}
			if (max_reg > max[1]/2 && max_reg > 1000)
			{
				c++;
			}
		}
		if (c > 0 )
		{
			HAL_UART_Transmit(&huart1 ,(uint8_t *)"Music!\n", 7 , HAL_MAX_DELAY);
		}
		else
		{
			HAL_UART_Transmit(&huart1 ,(uint8_t *)"Not Music!\n", 11 , HAL_MAX_DELAY);
		}
}
void Is_Music_OZWithInit(void )
{
	ADC1_Get_Music();
	for ( i = 0 ; i < FO_LENGTH_MIDDLE ; i++)
	{
		if (fabs(SP[i]) > max[4])
		{
			max[4] = SP[i];
		}
	}
	if (max[4] < 1.0 )
	{
		hold_weak++;
		hold_weak%=3;
		if(hold_weak ==1){
			USART1PutString("t4.txt=\"Weak Sound!\"\xff\xff\xff");

			HAL_GPIO_WritePin(GPIOA , GPIO_PIN_1 , GPIO_PIN_SET);
		}
		max[4] = -1;
		return ;
	}
	HAL_GPIO_WritePin(GPIOA , GPIO_PIN_1 , GPIO_PIN_RESET);
	for (i= 0 ; i < FO_LENGTH_MIDDLE ; i++)
	{
		SP[i]/=max[4] /10;
//		  sprintf(str_2 , "%.5f" , SP[i]);
//		  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
	}
	max[4] = 0;
	double r1 = 0.5;
	double r2 = 0.2 ;
	int a1 = OverOZ_Num_Get(r1 , FO_LENGTH_MIDDLE, SP);
	int a2 = 0;
	max[1] = 0;
	p =FFT_Mag_sqrt(FO_LENGTH_MIDDLE ,SP );
	for (int l = 0 ; l < FO_LENGTH_MIDDLE/2 ; l++)
			{
				if (max[1] < p[l] && l > 10 && l < 4000)
				{
					max[1] = p[l];
					//max_index[1] = l;
				}
//				sprintf(str_2 , "%.5lf" , p[l]);
//				HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//				HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
			}

	for (i = 0 ; i < FO_LENGTH_MIDDLE/2 ; i++)
	{
		if (p[i] > r2 * max[1])a2++;
	}

//	sprintf(str_2 , "%d" , a1);
//	HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 3   ,HAL_MAX_DELAY);
//	HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
//	sprintf(str_2 , "%d" , a2);
//	HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 4   ,HAL_MAX_DELAY);
//	HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
//	sprintf(str_2 , "%d" , 2);
	if ( a1 >90 || a2 < 50)
	{
		if (hold_music < 3)hold_music ++;
		hold_voice = ((hold_music >=2)?(5):(0));
	}
	if (a1 < 90  )
	{
		if (hold_music >=3){hold_voice--; }
		hold_music = 0;
	}
//	HAL_TIM_PWM_Stop(&htim4 , TIM_CHANNEL_2);
	if (hold_music >= 3 || hold_voice >= 5)
	{
		if (c == '7')
		{
			USART1PutString("t4.txt=\"Music Killed!\"\xff\xff\xff");
			kill_human = 0;
			kill_music ++;
			if(kill_music > 2)PWM_Output(40000 , 0.2);
			return ;
		}
		kill_music = 0;
		kill_human = 0;
		USART1PutString("t4.txt=\"Music!\"\xff\xff\xff");
		HAL_TIM_PWM_Stop(&htim4 , TIM_CHANNEL_2);
	}
	else
	{
		if (c == '6')
			{
			USART1PutString("t4.txt=\"Human Sound Killed\"\xff\xff\xff");
			kill_human++;
			kill_music = 0;
			if(kill_human > 2)PWM_Output(40000 , 0.2);
				return ;

			}
		kill_music = 0;
		kill_human = 0;
		USART1PutString("t4.txt=\"Human Sound!\"\xff\xff\xff");
		HAL_TIM_PWM_Stop(&htim4 , TIM_CHANNEL_2);
	}
//	HAL_Delay(10);
}

void ADC_FFT(void)
{
	flag = 0;
	Para_Init();
	ADC1_Get();
	//Input_Simulate();
	HandleFFT();
	PrintMag();
	if (c == '6' ||  c == '7'){f =1;}
	else {f = 0;}
	if (c == '1' || c == '0'){HAL_TIM_PWM_Stop(&htim4 , TIM_CHANNEL_2);}
}

//void PWM_Output(int frequency , double r)
//{
//	rate = r;
//	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
//	//WRITE_REG(TIM4->CCR1,(TIM4->ARR+1)*rate);
//	__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,(TIM4->ARR+1)*rate);
//	TIM4->ARR=1000000/frequency-1;
//	TIM4->CR1 &= ~TIM_CR1_ARPE;
//}




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
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM15_Init();
  MX_DAC1_Init();
  MX_ADC1_Init();
  MX_TIM5_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  Music_Init();
  Para_Init();
  //adc_init();
//  ADC1_Get();
  ASK_Output(1000);
//  PWM_Output(1000 , 0.2);
  //Is_Music();
  //ADC1_Get();
  HAL_GPIO_WritePin(GPIOA , GPIO_PIN_1 , GPIO_PIN_SET);
//  __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
  HAL_UART_Receive_IT(&huart1, (uint8_t*)&c,1);
//  PWM_Output();
//  for ( i = 0 ; i < 100; i++)
//  {
//	  ADC_FFT();
//	 HAL_UART_Transmi t(&huart1 ,(uint8_t *)"ok\n", 3 , HAL_MAX_DELAY);
//  }
  //ADC_FFT();
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//	  ADC1_Get_Music();
//	  Para_Init();
	  //ADC_FFT();
//	  PWM_Output(1000 , 0.2);
//	  Is_Music_OZWithInit();
//	  PWM_Output(20000 , 0.2);
	  //ADC_FFT();
//	  PWM_Output(40000 , 0.92);
	  if (f==0){ADC_FFT();}
	  else {Is_Music_OZWithInit();}
	  switch('c')
	  {
	  	case '0':
	  		HAL_TIM_PWM_Stop(&htim4 , TIM_CHANNEL_2);
//	  		ADC_FFT();
	  			  	 f= 0;
	  		break;
	  	case '1':
//	  		ADC_FFT();
	  		HAL_TIM_PWM_Stop(&htim4 , TIM_CHANNEL_2);
	  			  	 f=0;
	  		break;
	  	case '2':
//	  		ADC_FFT();
	  		PWM_Output(40000 , aaa);
	  			  	 f=0;
	  		break;
	  	case '3':
	  		PWM_Output(40000 , aaa);
//	  		ADC_FFT();
	  			  	 f=0;
	  		break;
	  	case '4':
	  		PWM_Output(40000 , aaa);
//	  		ADC_FFT();
	  			  	 f=0;
	  		break;
	  	case '5':
	  		PWM_Output(40000 , aaa);
//	  		ADC_FFT();
	  			  	 f=0;
	  		break;
	  	case '6':
	  		human_killed = 1;
	  		music_killed = 0;
	  		if (kill_human > 2){PWM_Output(40000 , 0.2);}
	  		else {HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);}
	  		f=1;
//	  		Is_Music_OZWithInit();
	  		break;
	  	case '7':
	  		human_killed = 0;
	  		music_killed = 1;
	  		if (kill_music > 2){PWM_Output(40000 , 0.2);}
	  		else {HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);}
	  		f= 1;
//	  		Is_Music_OZWithInit();
	  		break;
	  	default:
	  }
	  //HAL_UART_Receive_IT(&huart1, (uint8_t*)&c,1);
	  //PWM_Output(1000 , 0.2);
	  //ADC_FFT();
//	  Para_Init();
//	  Music_Init();
//	  Is_Music();
//	  HAL_UART_Receive_IT(&huart1, (uint8_t*)&c,1);
	  //HAL_Delay(10);
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
	 AdcConvEnd = 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(TIM5 == htim->Instance)
  {
        if(CNT)
    {
     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
     CNT=0;
    }
    else
    {
     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
     CNT=1;

    }
  }
    if (TIM4 == htim->Instance)
    {
    	WRITE_REG(TIM4->CCR1,(TIM4->ARR+1)*rate);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART1)
    {
        if(c=='0')
        {
        	f = 0;
        	aaa= 0.2;
        }
        if(c=='1')
        {
        	f = 0;
        	aaa=  0.2;
        }
         if(c=='2')
        {
        	 aaa+=0.1;
        		 PWM_Output(40000 , aaa);
        		 f = 0;

        }
        if(c=='3')
        {
        	aaa+=0.01;
        	     PWM_Output(40000 , aaa);
        	     f = 0;
        }
        if(c=='4')
        {
        	aaa-=0.1;
        		 PWM_Output(40000 , aaa);
        		 f = 0;
        }
        if(c=='5')
        {
        	aaa-=0.01;
        		PWM_Output(40000 , aaa);
        		f = 0;
        }
         if(c=='6')
        {
        	 human_killed=  1;
        	 music_killed = 0;
 	  		if (kill_human){PWM_Output(40000 , 0.2);}
 	  		else {HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);}
        	 f= 1;
        }
        if(c=='7')
        {
        	human_killed=  1;
        	music_killed = 0;
        	f = 1;
	  		if (kill_music){PWM_Output(40000 , 0.2);}
	  		else {HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);}
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
