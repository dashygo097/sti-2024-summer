/*
 * measure.h
 *
 *  Created on: Jul 22, 2024
 *      Author: dashygo
 */

#ifndef INC_MEASURE_H_
#define INC_MEASURE_H_

#define COUNT 1024

extern double R1 ;
extern double R2 ;

void LowPassFilter(double a[]);
double Calculate(double a[]);
void Get_Us();
void Get_Ui();
void Get_Uo1();
void Get_Uo2();
double Get_Rin();
double Get_Rout();
double Get_Av();
void fARelationship();
double FastGet_Uo1();
double FastGet_Us();
double FastGet_Ui();
double FastGet_Uo2();

#endif /* INC_MEASURE_H_ */
