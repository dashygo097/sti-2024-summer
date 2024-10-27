/*
 * STFT.c
 *
 *  Created on: Jul 31, 2024
 *      Author: dashygo
 */

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <string.h>
#include <string.h>
#include <math.h>
#include "usart.h"

#include "SSM.h"
#include "FO.h"

char str[50];

double sgn(double x)
{
	return (x > 0)?(1) : -1;
}

double OverZero_Ave_Get(int size , double x[])
{
    double Z = 0;

    for (int i = 1 ; i < size ; i++)
    {
        Z += fabs(sgn(x[i - 1]) - sgn(x[i]));
    }
    return Z / 2 / size;
}

double Energy(int size , double x[])
{
    double E = 0 ;
    for (int i = 0 ; i < size ; i++)
    {
        E += x[i] * x[i] ;
    }

    return E;
}

int FrameNumber_Get(int Fsize , int length , int motion )
{
    return (Fsize - length + motion ) /motion;
}

double MaxOZ_Get(int size , double x[])
{
    double *X = x;
    int num = FrameNumber_Get(size ,frame_length , frame_motion);
    double max = -1;
    for (int i = 0 ; i < num ; i++)
    {
        double Z  = OverZero_Ave_Get(frame_length , X + i* frame_motion);
        if (max < Z)
        {
            max = Z;
        }
    }

    return max;
}

int OverOZ_Num_Get(double para , int size , double x[])
{
    double* X = x;
    int count =  0;
    int num = FrameNumber_Get(size ,frame_length , frame_motion);

    double Z_max = MaxOZ_Get(size , x);

    for (int i= 0 ; i  < num ; i++)
    {
        double Z = OverZero_Ave_Get(frame_length,X + frame_motion*i);
        if (Z > Z_max * para)
        {
            count ++;
        }
//        sprintf(str , "%.5lf" , Z);
//        		  	    		  HAL_UART_Transmit(&huart1,(uint8_t *)str , 7   ,HAL_MAX_DELAY);
//        		  	    		  HAL_UART_Transmit(&huart1 ,(uint8_t *)"\n", 1 , HAL_MAX_DELAY);
    }

    return count ;
}





