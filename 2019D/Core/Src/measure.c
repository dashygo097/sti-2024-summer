/*
 * measure.c
 *
 *  Created on: Jul 22, 2024
 *      Author: dashygo
 */

#include "stdio.h"
#include "math.h"

#include "main.h"
#include "measure.h"
#include "gpio.h"
#include "usart.h"
#include "adc.h"
#include "signal.h"
#include "screen.h"

double R1 = 2, R2 = 2;
//char str_2[50] ;

void LowPassFilter(double a[])
{
	for (int i = 1; i < COUNT ; i++ )
	{
		a[i] = 0.03 * a[i] + 0.97* a[i-1];
	}
}


void LowPassFilter_Maintain(double a[])
{
	for (int i = 1; i < COUNT ; i++ )
	{
		a[i] = 0.97 * a[i] + 0.03* a[i-1];
	}
}

double Calculate(double a[])
{
    double num=0;
//    double MAX=a[0];
//    double MIN=a[0];
    for(int i=0;i<COUNT;i++)
    {
        num+=(a[i]*a[i]);
    }
   return sqrt(num/(double)COUNT) ;
}

 double FastGet_Us()
 {
	adc_init();
    double  Us_reg;
    double arrUs[COUNT];
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
    for(int i=0;i<COUNT;i++)
    {
     arrUs[i]=adc1_data[i*2]/65535.000000*3.3;
    // printf("%f\n",arrUs[i]);
 }

    Us_reg=Calculate(arrUs);
    LowPassFilter(arrUs);
    double dc = Calculate(arrUs);
    return sqrt(Us_reg*Us_reg - dc*dc);
 }
 double FastGet_Ui()
  {
 	adc_init();
     double  Us_reg;
     double arrUs[COUNT];
     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
     for(int i=0;i<COUNT;i++)
     {
      arrUs[i]=adc1_data[i*2]/65535.000000*3.3;
     // printf("%f\n",arrUs[i]);
  }

     Us_reg=Calculate(arrUs);
     LowPassFilter(arrUs);
     double dc = Calculate(arrUs);
     return sqrt(Us_reg*Us_reg - dc*dc);
  }
double FastGet_Uo1()
 {
    adc_init();
    double  Uo1_reg;
    double arrUo1[COUNT];
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
    for(int i=0;i<COUNT;i++)
    {
     arrUo1[i]=   adc1_data[i*2 + 1]/65535.000000*3.3;
    // printf("%f\n",arrUo1[i]);

 }
    Uo1_reg=Calculate(arrUo1);
    LowPassFilter(arrUo1);
    double dc = Calculate(arrUo1);
    return sqrt(Uo1_reg*Uo1_reg - dc*dc)/0.95;
 }

double FastGet_Uo2()
 {
    adc_init();
    double  Uo1_reg;
    double arrUo1[COUNT];
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET);
    for(int i=0;i<COUNT;i++)
    {
     arrUo1[i]=   adc1_data[i*2 + 1]/65535.000000*3.3;
    // printf("%f\n",arrUo1[i]);

 }
    Uo1_reg=Calculate(arrUo1);
    LowPassFilter(arrUo1);
    double dc = Calculate(arrUo1);
    return sqrt(Uo1_reg*Uo1_reg - dc*dc)/0.95;
 }

void Get_Us(void)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
	HAL_Delay(200);
	ADC1_Get();
//	LowPassFilter_Maintain(uin);
	int max_index[2];
	double max[2];
	double* s = FFT_ADC1(FO_LENGTH_LOW , uin ) ;
	max_index[1] = (*s) * 106;
	double s1 = *s;
	double * X;
	for (int i = 0 ; i < 5 ; i ++)
	{
		X= Blackman(FO_LENGTH_LOW ,(max_index[1]-max_index[1]/2),(max_index[1]+max_index[1]/2), uin);


		p = FFT_Mag(FO_LENGTH_LOW , X);
		for (int j = 0 ; j  <FO_LENGTH_LOW ; j++)
		{
			Pin[j]+=p[j];
		}
	}

	for (int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
		 {
			if (max[0] < Pin[i] &&  i < 20 && i >=0 )
			{
			  	max_index[0] = i ;
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
		 }
	for ( int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
			{
				Pin[i] = 0;
			}
	Us_DC= sqrt(max[0]) / s1  / 90.3;
	Us = sqrt(max[1])/ s1 / 22.9 /sqrt(2) /  2;
}
void Get_Ui(void)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
	HAL_Delay(200);
	ADC1_Get();
//	LowPassFilter_Maintain(uin);
	int max_index[2];
	double max[2];
	double* s = FFT_ADC1(FO_LENGTH_LOW , uin ) ;
	max_index[1] = (*s) * 106;
	double s1 = *s;
	double * X;
	for (int i = 0 ; i < 5 ; i ++)
	{
		X= Blackman(FO_LENGTH_LOW ,(max_index[1]-max_index[1]/2),(max_index[1]+max_index[1]/2), uin);


		p = FFT_Mag(FO_LENGTH_LOW , X);
		for (int j = 0 ; j  <FO_LENGTH_LOW ; j++)
		{
			Pin[j]+=p[j];
		}
	}

	for (int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
		 {
			if (max[0] < Pin[i] &&  i < 20 && i >=0 )
			{
			  	max_index[0] = i ;
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
		 }
	for ( int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
			{
				Pin[i] = 0;
			}
	Ui_DC= sqrt(max[0]) / s1  /  90.3;
	Ui = sqrt(max[1])/ s1 /  22.9 /sqrt(2) /  2;
}

 void Get_Uo1()
 {
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
    HAL_Delay(200);
	ADC1_Get();
	LowPassFilter_Maintain(uout);
	int max_index[2];
	double max[2];
	double* s = FFT_ADC1(FO_LENGTH_LOW , uout ) ;
	max_index[1] = (*s) * 106;
	double s1 = *s;
	double * X;
	for (int i = 0 ; i < 5 ; i ++)
	{
		X= Blackman(FO_LENGTH_LOW ,(max_index[1]-max_index[1]/2),(max_index[1]+max_index[1]/2), uout);


		p = FFT_Mag(FO_LENGTH_LOW , X);
		for (int j = 0 ; j  <FO_LENGTH_LOW ; j++)
		{
			Pin[j]+=p[j];
		}
	}

	for (int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
		 {
			if (max[0] < Pin[i] &&  i < 20 && i >=0 )
			{
			  	max_index[0] = i ;
			  	max[0] =Pin[i];

			}
			else if (i < 50 )
			{
			  	max_index[0] = max_index[0];
			  	max[0] = max[0];
			}
			if (max[1] < Pin[i] && i < (max_index[1]+20) &&  i >(max_index[1]-20) )
			{
			  	max_index[1] = i  ;
			  	max[1] =Pin[i];

			}
		 }
	for ( int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
			{
				Pin[i] = 0;
			}
	Uo1_DC= sqrt(max[0]) / s1  / 92.2;
	Uo1 = sqrt(max[1])/ s1 / 24.0 /0.955/sqrt(2) /  2;
}

 void Get_Uo2()
 {
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET);
    HAL_Delay(200);
    ADC1_Get();
    LowPassFilter_Maintain(uout);
    	int max_index[2];
    	double max[2];
    	double* s = FFT_ADC1(FO_LENGTH_LOW , uout ) ;
    	max_index[1] = (*s) * 106;
    	double s1 = *s;
    	double * X;
    	for (int i = 0 ; i < 5 ; i ++)
    	{
    		X= Blackman(FO_LENGTH_LOW ,(max_index[1]-max_index[1]/2),(max_index[1]+max_index[1]/2), uout);


    		p = FFT_Mag(FO_LENGTH_LOW , X);
    		for (int j = 0 ; j  <FO_LENGTH_LOW ; j++)
    		{
    			Pin[j]+=p[j];
    		}
    	}

    	for (int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
    		 {
    			if (max[0] < Pin[i] &&  i < 20 && i >=0 )
    			{
    			  	max_index[0] = i ;
    			  	max[0] =Pin[i];

    			}
    			else if (i < 50 )
    			{
    			  	max_index[0] = max_index[0];
    			  	max[0] = max[0];
    			}
    			if (max[1] < Pin[i] && i < (max_index[1]+20) &&  i >(max_index[1]-20) )
    			{
    			  	max_index[1] = i  ;
    			  	max[1] =Pin[i];

    			}
    		 }
    	for ( int i = 0 ; i < FO_LENGTH_LOW/2 ; i++)
    			{
    				Pin[i] = 0;
    			}
    	Uo2_DC= sqrt(max[0]) / s1  / 92.2;
    	Uo2 = sqrt(max[1])/ s1 / 24.0/0.955 /sqrt(2) /  2;
 }

double Get_Rin()
{
    return ( R1*Ui/(Us-Ui));
}

double Get_Rout()
{
    return R2*(Uo1-Uo2)/Uo2;
}

double Get_Av()
{
	return Us / Uo1;
}


void fARelationship()
{
	double Us_reg , Uo1_reg ,Gain;
    int frequency;
//    int upper_fre;
    for(int i=0;i<250;i++)
   {
    frequency=500*(i+1);
    AD9833_WaveSetting(frequency , 0 , SIN_WAVE , 0 , 0);

    Us_reg=FastGet_Us();
    Uo1_reg=FastGet_Uo1();
    Gain=20*log10(Uo1_reg/Us_reg);
    USART1PutString("add 8,0,");
    itostr(Gain,str_2);
    USART1PutString(str_2);
//    if(fabs(Uo1/Us-sqrt(2)/2)<0.05)
//    	{
//    	upper_fre=frequency;
//    	}
   }
}




