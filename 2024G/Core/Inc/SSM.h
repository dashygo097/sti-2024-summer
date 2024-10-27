/*
 * STFT.h
 *
 *  Created on: Jul 31, 2024
 *      Author: dashygo
 */

#ifndef INC_SSM_H_
#define INC_SSM_H_

#define frame_length 128
#define frame_motion 32

double sgn(double x);
double OverZero_Ave_Get(int size , double x[]);
double Energy(int size , double x[]);
int FrameNumber_Get(int Fsize , int length , int motion );
double MaxOZ_Get(int size , double x[]);
int OverOZ_Num_Get(double para , int size , double x[]);

#endif /* INC_SSM_H_ */
