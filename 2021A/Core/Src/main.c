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


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
ALIGN_32BYTES (uint16_t adc1_data[FO_LENGTH_HIGH]) 	__attribute__((section(".ARM.__at_0x30000000")));



__IO uint8_t AdcConvEnd = 0;

#define  LOW_FREQUENCY (uint8_t)99999
#define  HIGH_FREQUENCY (uint8_t)100001
#define RXBUFFERSIZE  256
char RxBuffer[RXBUFFERSIZE];

int max_index[7] = {0 ,0,0,0,0 , 0 ,0}  ,min_index[7] = {0,0,0,0,0,0,0};
double max[7] = {-1 , -1, -1 , -1, -1,-1,-1} , min[7] = {-1 , -1 , -1 , -1 , -1 ,-1,-1} ;
char str_2[50];
//double x[FO_LENGTH_HIGH + 100];
double * X ;
double *Y;
double *P;
double *p;
double voltage;
double v[FO_LENGTH_HIGH];
int i = 0;
int para = 0;
int flag = 0;
double last = 0;

double uo[6];
double THD;

double*s;

uint16_t GATE_OPEN,GATE_READY2CLOSE;
uint32_t Nx,Nb;
uint8_t Fre_State=LOW_FREQUENCY;
double fre;
uint8_t  capture_Cnt=0;


void itostr(int num)
{
	int sign = num,i = 0,j = 0;
	char temp[11];
	if(sign<0)
	{
		num = -num;
	}
	for(;num>0;)
	{
        temp[i] = num%10+'0';
		num/=10;
		i++;
	};
	if(sign<0)
	{
		temp[i++] = '-';
	}
	temp[i] = '\0';
	i--;
	while(i>=0)
	{
	str_2[j] = temp[i];
	j++;
	i--;
	}
	str_2[j] = '\0';
}
void PrintMag(void)
{
	 for (i = 0 ; i < FO_LENGTH_HIGH/2 ; i++)
		  	  {
//		  		  sprintf(str_2 , "%.5lf" , (P[i]));
		  		  if (max[0] < P[i] &&  i < 50 && i > 10 )
		  		  {
		  			  max_index[0] = i * 1 ;
		  		  	  max[0] =P[i];

		  		  }
		  		  else if (i < 100 )
		  		  {
		  		      max_index[0] = max_index[0];
		  		  	  max[0] = max[0];
		  		  }
		  		  for (int j = 1 ; j  <= 5 ; j ++)
		  		  {
		  			  if (max[j] < P[i] && i < (max_index[6]+20)*(j) &&  i >(max_index[6]-20)*(j) )
		  			  	  {
		  			  		  max_index[j] = i * 1 ;
		  			  	  	  max[j] =P[i];

		  			  	  }
		  		  }
//		  		HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		  		HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
		  	  }
}

void PrintTHD(void)
{
	  uo[0] = max[0];
	  double rate[5]={0};
	  for (i = 1 ; i <= 5; i++)
	  {
		uo[i] = (max[i]);

		rate[i-1]=sqrt(uo[i])/sqrt(uo[1]) * 100;


	  }
	  Oled_Init();
	  //Oled_Screen_Clear();
	  HAL_Delay(500);
	  Oled_WriteAsciiString("%%\n");
	  sprintf(str_2 , "Uo1/Uo1=%.2lf" , (rate[0]));
	  Oled_WriteAsciiString(str_2);
	  Oled_WriteAsciiString("%%\n");
	  	  sprintf(str_2 , "Uo2/Uo1=%.2lf" , (rate[1]));
	  	  Oled_WriteAsciiString(str_2);
	  	Oled_WriteAsciiString("%%\n");
	  	sprintf(str_2 , "Uo3/Uo1=%.2lf" , (rate[2]));
	  		  	  Oled_WriteAsciiString(str_2);
	  		  	Oled_WriteAsciiString("%%\n");
	  		  sprintf(str_2 , "Uo4/Uo1=%.2lf" , (rate[3]));
	  		Oled_WriteAsciiString(str_2);
	  		Oled_WriteAsciiString("%%\n");
	  		sprintf(str_2 , "Uo5/Uo1=%.2lf" , (rate[4]));
	  		Oled_WriteAsciiString(str_2);
	  		Oled_WriteAsciiString("%%\n");
//	  		  	  	  Oled_WriteAsciiString(str_2);
//	  		  	  	Oled_WriteAsciiString("\n");
//	  		  	sprintf(str_2 , "Uo2/Uo1=%.2lf" , (rate[4]));
//	  		  		  		  	  	  Oled_WriteAsciiString(str_2);
//	  		  		  		  	  	Oled_WriteAsciiString("\n");
//	  	Oled_WriteAsciiString("\n");
//	  		  sprintf(str_2 , "Uo3/Uo1=%.2lf" , (rate[2]));
//	  		  Oled_WriteAsciiString(str_2);
//	  		Oled_WriteAsciiString("\n");
//	  			  sprintf(str_2 , "Uo4/Uo1=%.2lf" , (rate[3]));
//	  			  Oled_WriteAsciiString(str_2);
//	  			Oled_WriteAsciiString("\n");
//	  				  sprintf(str_2 , "Uo5/Uo1=%.2lf" , (rate[4]));
//	  				  Oled_WriteAsciiString(str_2);
//	  				Oled_WriteAsciiString("\n");
	  sprintf(str_2 , "x2.val=%d\xff\xff\xff" ,(int) (rate[0] * 100));
	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 15 ,HAL_MAX_DELAY);
	  sprintf(str_2 , "x3.val=%d\xff\xff\xff" ,(int) (rate[1] * 100));
	  	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 21 ,HAL_MAX_DELAY);
	  	sprintf(str_2 , "x4.val=%d\xff\xff\xff" ,(int) (rate[2] * 100));
	  		  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 18,HAL_MAX_DELAY);
	  		sprintf(str_2 , "x5.val=%d\xff\xff\xff" ,(int) (rate[3] * 100));
	  			  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 21 ,HAL_MAX_DELAY);
	  			sprintf(str_2 , "x6.val=%d\xff\xff\xff" ,(int) (rate[4] * 100));
	  				  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 21 ,HAL_MAX_DELAY);

	  THD = sqrt(uo[5] + uo[2] + uo[3] + uo[4]) / (sqrt(uo[1])) * 100;
	  sprintf(str_2 , "x1.val=%d\xff\xff\xff" ,(int) (THD * 100));
	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 21 ,HAL_MAX_DELAY);
	  sprintf(str_2 , "THD=%.2lf" , THD);
	  	  Oled_WriteAsciiString(str_2);
	  	  Oled_WriteAsciiString("%%\n");
	  	HAL_Delay(500);
	for(i=0;i<8192;i++)
	{
//	    itostr((v[i]*10000/3)/10000);
//	    	  HAL_UART_Transmit(&huart1,(uint8_t *)"add 2,1,x" , 11 ,HAL_MAX_DELAY);
//	    	  HAL_UART_Transmit(&huart1, (uint8_t *)str_2,11,HAL_MAX_DELAY);
//        HAL_UART_Transmit(&huart1, (uint8_t *)"\xff\xff\xff",15,HAL_MAX_DELAY);
   }

	}



void Para_init(void)
{
	for (i = 0 ; i < 6 ; i ++)
		  {
			  max[i] = 0;
			  max_index[i] = 0;

		  }
		  for ( i = 0 ; i < FO_LENGTH_HIGH ; i++)
		  {
			  P[i] = 0;
		  }
}

void adc_init(void)
{
	//HAL_UART_Transmit(&huart1,(uint8_t *)"start_adc\n",sizeof("start_adc\n"),HAL_MAX_DELAY);
	MX_ADC1_Init();
	HAL_Delay(300);
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

void ADC1_Get(int pa , double con)
{
	//	Fre_Get_TIM();
		HAL_Delay(300);

		adc_init();

		  for (uint16_t temp = 0; temp< FO_LENGTH_HIGH;temp++)
		  	    {
		  	    	  voltage = ((double)adc1_data[temp]*3.3)/65535 *3 ;

		  	    	  v[temp] = voltage;
		  	    	  sprintf(str_2 , "%.5f" , v[temp]);
		  	    	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
		  	    	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
		  	    }

		  MX_TIM15_Init();
		  HAL_Delay(100);
		  //X = FFT(FO_LENGTH_HIGH , v) ;
		  s = FFT_ADC1(FO_LENGTH_HIGH , v , pa) ;
		  max_index[6] = (*s) * con;
}

void HandleFFT(void)
{
//				  sprintf(str_2 , "%.2f" , *s);
//				  HAL_UART_Transmit(&huart1 ,(uint8_t *)"Frequency:", 10 , HAL_MAX_DELAY);
//			  	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 5   ,HAL_MAX_DELAY);
//			  	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"kHz\n", 4 , HAL_MAX_DELAY);
			  	  for (i = 1 ; i <= 5; i++)
			  	  {
			  		  if (flag == 1)
			  		  {
			  			X= Blackman(FO_LENGTH_HIGH ,(max_index[6]-max_index[6]/2)*i,(max_index[6]+max_index[6]/2)*i, v);
			  		  }
			  		  else
			  		  {
			  			  X = v;
			  		  }

			  		  p = FFT_Mag(FO_LENGTH_HIGH , X);
			  		  for (int j = 0 ; j  <FO_LENGTH_HIGH ; j++)
			  		  {
			  			  P[j]+=p[j];
			  		  }
			  	  }
			  	  //P = FFT_Bartlett(FO_LENGTH_HIGH , 16 ,  x);

			  	  PrintMag();
			  	  PrintTHD();
			  	  Para_init();
}

void ADC_FFT(void)
{
      //Oled_Init();
      //Oled_Screen_Clear();
	  HAL_UART_Transmit(&huart1,(uint8_t *)"start_adc\n",sizeof("start_adc\n"),HAL_MAX_DELAY);
	  Para_init();
	  para = 1;
	  HAL_TIM_Base_Stop(&htim15);
	  MX_MyTIM15_Init(para);
	  HAL_TIM_Base_Start(&htim15);
	  ADC1_Get(1,210);
	  Para_init();
	  if (max_index[6] < 750)
	  {
		  HandleFFT();
	  }
	  else
	  {
		  HAL_Delay(100);
		  Para_init();
		  para = 2;
		  HAL_TIM_Base_Stop(&htim15);
		  MX_MyTIM15_Init(para);
		  HAL_TIM_Base_Start(&htim15);
		  ADC1_Get(2 , 56.75 );
		  if (max_index[6] < 750  )
		  {
			  HandleFFT();
		  }
		  else
		  {
			  HAL_Delay(100);
			  Para_init();
			  para = 3;
			  HAL_TIM_Base_Stop(&htim15);
			  MX_MyTIM15_Init(para);
			  HAL_TIM_Base_Start(&htim15);
			  ADC1_Get(3 , 14.6);
			  if (max_index[6] < 750)
			  {
				  HandleFFT();
			  }
			  else
			  {
				  HAL_Delay(100);
				  Para_init();
				  para = 4;
				  HAL_TIM_Base_Stop(&htim15);
				  MX_MyTIM15_Init(para);
				  HAL_TIM_Base_Start(&htim15);
				  ADC1_Get(4 , 7.085);
				  HandleFFT();
			  }
		  }
	  }
	  if (THD < 10 || THD > 55)
	  {
		  flag = 0;
	  }
	  else
	  {
		  flag = 1;
	  }
}

void Fre_Get_TIM(void)
{
//	GATE_OPEN=0;
//		GATE_READY2CLOSE=0;
//		Nx=0;
//		Nb=0;

	if(capture_Cnt==0)
	{
		capture_Cnt++;
	    __HAL_TIM_SET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
	    HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1);         //启动输入捕获
	    GATE_OPEN=0;
	    		GATE_READY2CLOSE=0;
	    		Nx=0;
	    		Nb=0;
	}
	if(capture_Cnt==2)
	{
	    capture_Cnt=0;
	    fre = fre*100/99998;
	    sprintf(str_2 , "%.5lf" , fre );
	    HAL_UART_Transmit(&huart1,(uint8_t *)"Frequency:" , sizeof("Frequency:")   ,HAL_MAX_DELAY);
	    HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7  ,HAL_MAX_DELAY);
	    HAL_UART_Transmit(&huart1,(uint8_t *)"kHz" , sizeof("kHz")   ,HAL_MAX_DELAY);
	    HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
	    HAL_TIM_Base_Stop(&htim15);
	    MX_MyTIM15_Init();
	    HAL_Delay(100);
	    //HAL_TIM_Base_Start(&htim15);
	    //htim15.Init.Period = (int)floor(400/fre  + 0.5);//1kHz->400 , 100kHz -> 4
	    //sprintf(str_2 , "%d" , (htim15.Init.Period));
	    HAL_UART_Transmit(&huart1,(uint8_t *)"TIM:" , 4 ,HAL_MAX_DELAY);
	    HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7 ,HAL_MAX_DELAY);
	    HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
	    HAL_Delay(500);
	    GATE_OPEN=0;
	    	GATE_READY2CLOSE=0;
	    	Nx=0;
	    	Nb=0;
	}
}

void Fre_Get_ADC()
{
//	adc_init();
//		  adc2_init();
//		  adc3_init();
//		  HAL_Delay(200);
//		    for (uint16_t temp = 0; temp< FO_LENGTH_HIGH;temp++)
//		    {
//		    	  voltage = ((double)adc1_data[temp]*3.3)/65535 * 4 ;
//		    	  voltage2 = ((double)adc2_data[temp]*3.3)/65535  * 4;
//		    	  voltage3 = ((double)adc3_data[temp]*3.3)/65535 * 4 ;
//		    	  v[temp] = voltage;
//		    	  v2[temp] = voltage2;
//		    	  v3[temp] = voltage3;
//		    	  sprintf(str_2 , "%.5f" , v2[temp]);
//		    	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//		    	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
//		    }
//	s = FFT_ADC1(FO_LENGTH_HIGH , v) ;
//
//		    if ( *s >= 100.2)
//		    {
//		  	  sprintf(str_2 , "%.5lf" , *s);
//		  	  HAL_UART_Transmit(&huart1,(uint8_t *)"Frequency:" , sizeof("Frequency:")   ,HAL_MAX_DELAY);
//		  	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 6  ,HAL_MAX_DELAY);
//		  	  HAL_UART_Transmit(&huart1,(uint8_t *)"MHz" , sizeof("MHz")   ,HAL_MAX_DELAY);
//		  	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
//		    }
//		  	else
//		  	{
//		  		s = FFT_ADC2(FO_LENGTH_HIGH , v2);
//		  		if (*s >=100.0 )
//		  		{
//		  			sprintf(str_2 , "%.5lf" , *s);
//		  			HAL_UART_Transmit(&huart1,(uint8_t *)"Frequency:" , sizeof("Frequency:")   ,HAL_MAX_DELAY);
//		  			HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 6  ,HAL_MAX_DELAY);
//		  			HAL_UART_Transmit(&huart1,(uint8_t *)"MHz" , sizeof("MHz")   ,HAL_MAX_DELAY);
//		  		    HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
//		  		}
//		  		else
//		  		{
//		  			s = FFT_ADC3(FO_LENGTH_HIGH , v3);
//		  			if (*s >= 1100)
//		  			{
//		  				sprintf(str_2 , "%.5lf" , *s);
//		  				HAL_UART_Transmit(&huart1,(uint8_t *)"Frequency:" , sizeof("Frequency:")   ,HAL_MAX_DELAY);
//		  				HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 6  ,HAL_MAX_DELAY);
//		  				HAL_UART_Transmit(&huart1,(uint8_t *)"kHz" , sizeof("kHz")   ,HAL_MAX_DELAY);
//		  				HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
//		  			}
//		  			else{Fre_Get_TIM();}
//		        }
//		    }
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

void Turn2HIGH_FREQUENCY_MODE(void);
void Turn2LOW_FREQUENCY_MODE(void);
void MX_TIM5_Init_ETR(void);
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
  MX_TIM2_Init();
  MX_DAC1_Init();
  MX_TIM5_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  Oled_Init();
  __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
  __HAL_TIM_CLEAR_IT(&htim5, TIM_IT_UPDATE);

//
//  adc_init();
//
//
//  for (uint16_t temp = 0; temp< FO_LENGTH_HIGH;temp++)
//  	    {
//  	    	  voltage = ((double)adc1_data[temp]*3.3)/65535 *2 ;
//
//  	    	  v[temp] = voltage;
//  	    	  sprintf(str_2 , "%.5f" , v[temp]);
//  	    	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7   ,HAL_MAX_DELAY);
//  	    	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
//  	    }
//  HAL_Delay(100);
//  X = FFT(FO_LENGTH_HIGH , v) ;
//  P = FFT_Mag(FO_LENGTH_HIGH  , v);
//  for (i = 0 ; i < FO_LENGTH_HIGH ; i++)
//  {
//	  sprintf(str_2 , "%.5lf" , (P[i]));
//	  if (max[0] < P[i] &&  i < 100 && i > 10 )
//	  {
//		  max_index[0] = i * 1.0 ;
//	  	  max[0] =P[i];
//
//	  }
//	  else if (i < 100 )
//	  {
//	      max_index[0] = max_index[0];
//	  	  max[0] = max[0];
//	  }
//	  for (int j = 1 ; j  <= 5 ; j ++)
//	  {
//		  if (max[j] < P[i] && i < 720*(j) &&  i > 680*(j) )
//		  	  {
//		  		  max_index[j] = i * 1.0 ;
//		  	  	  max[j] =P[i];
//
//		  	  }
//		  else if (i < 800*(j) &&  i > 720*(j))
//		  	  {
//		  		  max_index[j] = max_index[j];
//		  	  	  max[j] = max[j];
//		  	  }
//	  }
//		  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7 ,HAL_MAX_DELAY);
//		  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
//  }
//  for (i = 0 ; i <= 5; i++)
//  {
//	  uo[i] = (max[i]);
//	  HAL_UART_Transmit(&huart1,(uint8_t *)"Uo" , 2 ,HAL_MAX_DELAY);
//	  sprintf(str_2 , "%d" , i );
//	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 1 ,HAL_MAX_DELAY);
//	  HAL_UART_Transmit(&huart1,(uint8_t *)":" , 1 ,HAL_MAX_DELAY);
//	  sprintf(str_2 , "%.5lf" , sqrt(uo[i]));
//	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7 ,HAL_MAX_DELAY);
//	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
//  }
//
//  THD = sqrt(uo[5] + uo[2] + uo[3] + uo[4]) / (sqrt(uo[1]));
//  sprintf(str_2 , "%.5lf" , THD);
//  HAL_UART_Transmit(&huart1,(uint8_t *)"THD:" , 4 ,HAL_MAX_DELAY);
//  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7 ,HAL_MAX_DELAY);
//  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  ADC_FFT();
	   // sprintf(str_2 , "%.5lf" , 0);
//	    HAL_UART_Transmit(&huart1,(uint8_t *)"0" , 1 ,HAL_MAX_DELAY);
//	    HAL_UART_Transmit(&huart1,(uint8_t *)"1" , 1 ,HAL_MAX_DELAY);
//	    HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
	  //HAL_Delay(1000);
	  //Oled_Screen_Clear();
	  //Fre_Get_TIM();

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

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
 //printf("捕获回调\n");
	if(!GATE_OPEN)
	{
		//启动闸门 TIM5 �??始计数标准信�??
		__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim4);
		HAL_TIM_Base_Start(&htim2);
		GATE_OPEN=1;
	}
	else{
		//测频中，捕捉到一次信�?? 输入计数值加1
		Nx++;
		if(Nx>10000){
			HAL_TIM_Base_Stop(&htim2);
			HAL_TIM_IC_Stop_IT(&htim5, TIM_CHANNEL_1);
			HAL_TIM_Base_Stop_IT(&htim4);
            Turn2HIGH_FREQUENCY_MODE();
		}
	}

	if(GATE_OPEN&&GATE_READY2CLOSE)
	{
		//关闭闸门 停止捕捉
		HAL_TIM_Base_Stop(&htim2);
		HAL_TIM_IC_Stop_IT(&htim5, TIM_CHANNEL_1);
		HAL_TIM_Base_Stop_IT(&htim4);
		//锁存Nx Nb数据 计算频率
		Nb=__HAL_TIM_GetCounter(&htim2);
		fre=((double)Nx)/((double)Nb)*240000000.000000;
		//判断
		if(fre<10000.0)
		{
			capture_Cnt++;
			//准备下一次测
			__HAL_TIM_SetCounter(&htim2,0);  //计数不会因为定时器暂停�?�清
		}
		else
		{
            Turn2HIGH_FREQUENCY_MODE();
		}
	}
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//闸门溢出 规定测量时间
	if(TIM4 == htim->Instance&&Fre_State==LOW_FREQUENCY)
    {
			GATE_READY2CLOSE=1;
    }
	else if(TIM4 == htim->Instance&&Fre_State==HIGH_FREQUENCY)
    {
			HAL_TIM_Base_Stop_IT(&htim4);
			HAL_TIM_Base_Stop_IT(&htim5);
			HAL_TIM_Base_Stop(&htim2);
			Nx+=TIM5->CNT;
			Nb=TIM2->CNT;
	    	fre=((double)Nx)/((double)Nb)*240000000.000000;
			if(fre>=10000.0)
			{
			  capture_Cnt++;
	  	      __HAL_TIM_SetCounter(&htim2,0);
			  __HAL_TIM_SetCounter(&htim5,0);
			}
			else
			{
				Turn2LOW_FREQUENCY_MODE();
			}
    }
}

void Parameter_Init(void)
{

	GATE_OPEN=0;
	GATE_READY2CLOSE=0;
	Nx=0;
	Nb=0;
}

void Turn2HIGH_FREQUENCY_MODE(void)
{
   // printf("高频测量\n");
		    Fre_State=HIGH_FREQUENCY;
			MX_TIM5_Init_ETR();

			Parameter_Init();
	  	   __HAL_TIM_SetCounter(&htim2,0);
			__HAL_TIM_SetCounter(&htim5,0);

			HAL_TIM_Base_Start_IT(&htim4);
			HAL_TIM_Base_Start_IT(&htim5);
			HAL_TIM_Base_Start(&htim2);
}

void Turn2LOW_FREQUENCY_MODE(void)
{
   // printf("低频测量\n");
		Fre_State=LOW_FREQUENCY;
		Parameter_Init();
	    __HAL_TIM_SetCounter(&htim2,0);
		MX_TIM5_Init();
		HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1);    //启动输入捕获
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
