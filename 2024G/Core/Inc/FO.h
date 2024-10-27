
#ifndef INC_FO_H
#define INC_FO_H

#include <complex.h>

double complex* DFT(int size , double* x);
double complex* FFT(int size , double* x);
int rev(int size , int index);
double complex*rev_copy(int size , double* a);
double * FFT_Mag(int size , double x[]);
double* FFT_Bartlett(int size ,int div , double x[]);
double* Hamming(int size ,int left , int right ,double para , double x[]);
double* FFT_Welch(int size ,int div, double para  , double x[]);
double* Square_Window(int size ,int left , int right ,double x[]);
double* Blackman(int size , int left , int right , double x[]);
double * FFT_Mag_sqrt(int size , double x[]);


#endif
