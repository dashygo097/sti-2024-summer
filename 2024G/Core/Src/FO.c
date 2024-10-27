#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "FO.h"
#include "signal.h"
#include "main.h"
#include "oled.h"
#include "adc.h"
#include "usart.h"


double complex* DFT(int size ,double x[] )
{
    double complex X[size + 100];
    double complex * A =X;
    if (size < 0 ){}
    else 
    {
        for (int m = 0; m < size ;m++)
        {
            X[m] = x[0] + 0*I;
            for (int n = 1 ; n < size ;n++)
            {
                X[m] += x[n] * (cos(2.0*M_PI*m*n/size) - I*sin(2.0*M_PI*m*n/size));
            } 
        }
    }
    return A;
}

int rev(int size , int index)
{
    int k = floor(log2(size)) , reg = 0;
    int ans = 0;
    for (int i =size/2 ; i>0 ; i/=2)
    {
        reg = index%2;
        ans += reg * pow(2 , k - 1) ;
        k--;
        index/= 2;
    }
    return ans;
}


double complex* rev_copy(int size , double a[])
{
    double complex A[size + 100];
    double complex*X = A;
    for (int i = 0 ; i < size ; i++)
    {
        A[i] = a[rev(size,i)] + 0.0*I;
    }
    return X;
}

double complex* FFT(int size , double x[])
{
    double complex W[size/2];

    for (int i = 0 ; i < size/2 ; i++)
    {
        W[i] = cos(2*M_PI*i/size) + I*sin(2*M_PI*i/size);
    }

    double complex A[size +100];
    double complex *X =A;
    for (int i = 0;i < size ; i ++)
    {
    	A[i] = x[rev(size,i)] + 0*I;
    }
    
    int s , m , k , j;
    double complex omega , t , u;

    for(s = 1 ; s <size ; s*=2)
    {
        m = s*2;
        for ( k = 0 ; k < size ; k+=m)
        {
            omega = 1;
            for (j = 0; j < m/2 ; j++)
            {
                t = omega * A[k+j+m/2];
                u = A[k+j];
                A[k+j] = u + t;
                A[k+j+m/2] = u - t;
                omega = omega * W[size/m];
            } 
        }
    }
    return X;

}

double * FFT_Mag(int size , double x[])
{
    double A[size +100];
    double cr , ci;
    double complex*X = FFT(size , x);
    for (int i = 0 ; i < size ; i++)
    {
        cr = (creal(X[i]) * creal(X[i]));

        ci = (cimag(X[i]) * cimag(X[i]));
        A[i] = (cr + ci) / size;
    }
    double* a = A;
    return a;
}


double * FFT_Mag_sqrt(int size , double x[])
{
    double A[size +100];
    double complex*X = FFT(size , x);
    for (int i = 0 ; i < size ; i++)
    {
        A[i] = cabs(X[i]) ;
    }
    double* a = A;
    return a;
}

double* Square_Window(int size ,int left , int right ,double x[])
{
    double A[size +100];
    double * a = A;
    for (int i = 0 ; i < size ; i ++)
    {
        if (i <= right && i >= left)
        {
            A[i] = 10*x[i];
        }
        else
        {
            A[i] = 0;
        }
    }
    return a;
}


double* FFT_Bartlett(int size ,int div , double x[])
{
    double P[size + 100] ;
    double * p =P;
    double complex* X;
    for (int i = 0; i < div ; i++)
    {
        X = FFT(size , Square_Window(size,size/div * i, size/div *(i+1), x));
        for (int j = 0 ; j < size ; j++)
        {
            P[j] += creal(X[j]) * creal(X[j])/div + cimag(X[j]) * cimag(X[j])/div ;
        }

    }

    return p;

}


double* Hamming(int size , int left , int right , double para , double x[])
{
    double A[size  +100];
    double *a = A ;

    for (int i = 0 ; i < size ; i++)
    {
    	if (i < right && i >=  left)
    	{
    		A[i] = 10* x[i] * ((1 - para) - para* cos(2*M_PI*(i - left )/(right - left - 1)));
    	}
    	else
    	{
    		A[i] = 0;
    	}
    }
    return a;
}



double* FFT_Welch(int size ,int div , double para, double x[])
{
    double P[size + 100] ;
    double complex* X;
    for (int i = 0; i < div * 2 - 1 ; i++)
    {
        X = FFT(size , Hamming(size,(size/div/2)*(i) ,(size/div/2)*(i+2) , para , x));
        for (int j = 0 ; j < size ; j++)
        {
           P[j] += creal(X[j]) * creal(X[j])/size /2 /div+ cimag(X[j]) * cimag(X[j])/size / 2 / div;
        }
    }
    double * p =P;

    return p;
}

double* Blackman(int size , int left , int right , double x[])
{
    double A[size  +100];
    double *a = A ;

    for (int i = 0 ; i < size ; i++)
    {
    	if (i < right && i >=  left)
    	{
    		A[i] = 10* x[i] * ( 0.42 + 0.08* cos(4*M_PI*(i - left )/(right - left - 1)) - 0.5*cos(2*M_PI*(i - left )/(right - left - 1)));
    	}
    	else
    	{
    		A[i] = 0;
    	}
    }
    return a;
}




