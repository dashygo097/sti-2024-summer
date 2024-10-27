/*
 * cart.h
 *
 *  Created on: Jul 22, 2024
 *      Author: dashygo
 */

#ifndef INC_CART_H_
#define INC_CART_H_

// 定义训练数据结构
struct TrainingData {
    int R_in;
    int R_out;
    int A_v;
    int Vdc
    int State;
};

// 定义决策树节点结构
struct TreeNode {
    int attribute;
    int decision;
    struct TreeNode* left;
    struct TreeNode* right;
    struct TreeNode* middle;
};



#endif /* INC_CART_H_ */
