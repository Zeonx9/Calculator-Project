#ifndef CPROJECTSCALCULATOR_CALC_TREE_H
#define CPROJECTSCALCULATOR_CALC_TREE_H

#include "input_parse.h"
#include <complex.h>

// struct of one node of calculation tree has data of given element and pointers to its operands
typedef struct tree_node {
    Token data;
    struct tree_node *left;
    struct tree_node *right;
} TreeNode;

// recursively build a calculation tree
// search for the last-evaluated operation of function and then
// call makeTree() function on each of its operands
TreeNode * makeTree(TokenArray expr);

// evaluate a single line using pre-build tree of it
complex double calcExpr(TreeNode * tree, complex double * variables, int * initialized);

// calculate whole expression by performing calcExpr() on each line
complex double calculate(ParsedExpression pe);

#endif //CPROJECTSCALCULATOR_CALC_TREE_H
