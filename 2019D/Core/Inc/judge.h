/*
 * analyse.h
 *
 *  Created on: Jul 24, 2024
 *      Author: dashygo
 */

#ifndef INC_JUDGE_H_
#define INC_JUDGE_H_

#define No_Error 0

#define R1_To0 1
#define R2_To0 2
#define R3_To0 3
#define R4_To0 4

#define R1_ToInf 5
#define R2_ToInf 6
#define R3_ToInf 7
#define R4_ToInf 8

#define C1_ToInf 9
#define C2_ToInf 10
#define C3_ToInf 11

#define C1_Mul2 12
#define C2_Mul2 13
#define C3_Mul2 14

int Get_State(void);
void State_Out(int state);

#endif /* INC_JUDGE_H_ */
