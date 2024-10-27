
#ifndef INC_SIGNAL_H
#define INC_SIGNAL_H

#define FO_LENGTH_LOW 1024
#define FO_LENGTH_MIDDLE 4096
#define FO_LENGTH_HIGH 1024

#include <complex.h>
#include "main.h"

double* Sinewave_GetSample(double f , double amp , double phi , double f_get);
double* Squarewave_GetSample(double f ,double amp , double phi , double f_get);
uint32_t* Sinewave_GetFrequence(double x[] );
double* FFT_ADC1(int size , double *x );
double* FFT_ADC2(int size , double *x);
double* FFT_ADC3(int size , double *x);
double * RC_re(int size , int para , double x[] );

#endif

