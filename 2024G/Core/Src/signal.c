#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "signal.h"
#include "main.h"
#include "oled.h"
#include "adc.h"
#include "usart.h"


double hadc1_sample_fre = 120000000 / 1.5 / 1 / 16 / 50 * 2.96 * 100;

double* Sinewave_GetSample(double f , double amp , double phi , double f_get)
{
    int size = ceil(f_get / f) ; 
    double x[FO_LENGTH_HIGH];
    double *A = x;
    for (int k=0 ;k < size ; k++)
    {
        x[k] =amp* sin(phi/180*M_PI +2* M_PI*k*(f/f_get));
    }
    return A;
}

double* Squarewave_GetSample(double f ,double amp , double phi , double f_get)
{
    int size = ceil(f_get / f) ;
    double x[FO_LENGTH_HIGH];
    double * A =x;

    for (int k = 0 ; k < size ; k++)
    {
        x[k] = amp * ((sin(phi/180 * M_PI + 2*M_PI * k * (f/f_get))) >= 0);
    }
    return A;
}

uint32_t* Sinewave_GetFrequence(double x[])
{
	double complex *X = FFT(FO_LENGTH_HIGH , x);
	uint32_t index[5] = {-1,-1,-1,-1 ,-1} ;
	uint32_t *ptr = index;
	int k = 0;
	for (uint32_t i = 1 ; i < FO_LENGTH_HIGH ; i++)
	{
		if(((cimag(X[i]) > 200 )|| (cimag(X[i]) < -200)) && k < 5)
		{
			index[k] =  i ;
			k++ ;
		}
		else {continue;}
	}
	return ptr;
}

double* FFT_ADC1(int size , double *x )
{
	//char str_2[50];
	int i = 0 , max_index = 0  ,min_index = 0;
	double max , min ;
	double  s1;
	double *s = &s1;
	double complex *X =FFT(FO_LENGTH_LOW, x);
	  for (i = 0 ; i < FO_LENGTH_LOW ; i++)
	  {
//		  sprintf(str_2 , "%.5lf" , cabs(X[i]));
		  if (max < cabs(X[i]) && i < FO_LENGTH_LOW/2 &&  i > 20 && cabs(X[i]) > 20)
		  {
			  max_index = i * 1.0 ;
			  max =cabs(X[i]);

		  }
		  else if (min > cabs(X[i]) && i < FO_LENGTH_LOW/2 && i > 20 && cabs(X[i]) <-20)
		  {
			  min_index = i ;
			  min = cabs(X[i]);
		  }
		  else
		  {
			  max_index = max_index;
			  max = max;
			  min_index = min_index ;
			  min = min;
		  }

//		  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7 ,HAL_MAX_DELAY);
//		  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
	  }
	  double index = (max_index - min_index < 5)?(max_index + min_index ) : (2*max_index);
	  s1 = index*4.69   / 1024;
//	  sprintf(str_2 , "%d" , max_index);
//	  HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7 ,HAL_MAX_DELAY);
//	  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
	  return s ;
}

double * RC_re(int size , int para , double x[] )
{
	double v[size + 100] ;
	double * X = v;
	if (para == 1)
	{
		v[0] = x[0];
		for (int i = 1 ; i < size ; i++)
		{
			v[i] = 0.03 * x[i] + 0.97 * v[i - 1];
		}
	}
	else if (para == 2)
	{

	}
	else if (para == 3)
	{

	}
	else if (para == 4)
	{

	}
	return X;
}


double* FFT_ADC2(int size , double *x)
{
	//char str_2[50];
	int i = 0 , max_index = 0  ,min_index = 0;
	double max , min ;
	double  s1;
	double *s = &s1;
	double complex *X =FFT(FO_LENGTH_HIGH, x);
	  for (i = 0 ; i < FO_LENGTH_HIGH ; i++)
	  {
		  //sprintf(str_2 , "%.5lf" , cimag(X[i]));
		  if (max < cimag(X[i]) && i < 4096 &&  i > 50 && cimag(X[i]) > 20)
		  {
			  max_index = i * 1.0 ;
			  max =cimag(X[i]);

		  }
		  else if (min > cimag(X[i]) && i < 4096 && i > 50 && cimag(X[i]) < -20)
		  {
			  min_index = i ;
			  min = cimag(X[i]);
		  }
		  else
		  {
			  max_index = max_index;
			  max = max;
			  min_index = min_index ;
			  min = min;
		  }

		  //HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7 ,HAL_MAX_DELAY);
		  //HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
	  }
	  double index = (max_index - min_index < 5)?(max_index + min_index ) : (2*max_index);
	  s1 =  index*1.2 / (8192)   ;
	  return s ;
}



double* FFT_ADC3(int size , double *x)
{
	double complex *Y =FFT(FO_LENGTH_HIGH, x);
	//char str_2[50];
    int i = 0 , max_index3 = 0  ,min_index3 = 0;
	double max3 , min3 ;
	double  s3;
	double *s = &s3;
	for (i = 0 ; i < FO_LENGTH_HIGH ; i++)
	{
		//sprintf(str_2 , "%.5lf" , cimag(Y[i]));
		if (max3 < cimag(Y[i]) && i < 4096 &&  i > 200 && cimag(Y[i]) > 50)
		{
			max_index3 = i * 1.0 ;
		  	max3 =cimag(Y[i]);

		}
		else if (min3 > cimag(Y[i]) && i < 4096 && i > 200 && cimag(Y[i]) < -50)
		{
		  	min_index3 = i ;
		  	min3 = cimag(Y[i]);
		}
		else
		{
		  	max_index3 = max_index3;
		  	max3 = max3;
		  	min_index3 = min_index3 ;
		  	min3 = min3;
		}
		//HAL_UART_Transmit(&huart1,(uint8_t *)str_2 , 7 ,HAL_MAX_DELAY);
		//HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n" , 1 , HAL_MAX_DELAY);
		}
		double index3 =(max_index3 - min_index3 < 5)?(max_index3 + min_index3 ): (2*max_index3);
		s3 = index3 *1200 / (8192) ;
		return s;
}

