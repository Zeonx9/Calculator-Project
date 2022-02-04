#define T

#include <malloc.h>
#include <assert.h>
#include <complex.h>
#include "calc_tree.h"
#include "func_wraps.h"
#include "tests_print.h"
#ifdef T

// prioritize operations
int priority(Token * t) {
    // in descending order:
    // functions > ^ > *,/ > +,- > , > =
    if (t->type == identifier && t->act != none) return 6;
    if (t->type != operation) return -1;
    switch (t->act) {
        case eqv: return 1;
        case cma: return 2;
        case add: case sub: return 3;
        case mul: case dev: return 4;
        case car: return 5;
        default: return -1;
    }
}

// unary minus is only in the start of expression or after operator except close bracket
int isUnaryMinus(Token *t) {
    return t->act == sub && ((t - 1)->type == operation && (t - 1)->act != brc);
}

// find the operator which will be evaluated last and return its index if no operators return -1
int indexOfRootOp(TokenArray expr) {
    int ind = -1, curOpDepth = 0, curOpPriority = 0, depth = 0;
    // for each token if it is operator check its priority and bracket-depth
    for (Token *cur = expr.array; cur < expr.array + expr.size; ++cur)
        if (!isConstant(cur)) {
            if (cur->act == bro) ++depth;
            if (cur->act == brc) --depth;
            if (priority(cur) < 0) continue;
            // if no operator found or current operator is lower depth or same but lower priority then update
            if (ind < 0 || (depth < curOpDepth || (curOpDepth == depth && priority(cur) <= curOpPriority))) {
                ind = (int) (cur - expr.array);
                curOpDepth = depth;
                curOpPriority = priority(cur);
            }
        }
    return ind;
}

// find and return operand of operation "toRight" argument define direction of search if 0 then to left
TokenArray getOperand(TokenArray expr, int index, int toRight, int bracketed) {
    int depth = 0, i;
    // if bracket which break correct sequence or end reached then exit loop
    for ( i = index; i < expr.size && i >= 0; toRight ? ++i : --i)
        if (isOperator(expr.array + i)) {
            if (expr.array[i].act == bro) toRight ? ++ depth : --depth;
            if (expr.array[i].act == brc) toRight ? -- depth : ++depth;
            if (bracketed ? depth ==  0 : depth == -1 ) break;
        }
    // find length of operand
    int len = (toRight ? i - index : index - i) - 1;
    if (len < 1) {
        printf("Error: No arguments passed1!\n");
        exit(EMPTY_BRACKETS);
    }
    return toRight ? (TokenArray) {len, expr.array + index + 1} :
           (TokenArray) {len, expr.array + index - len};
}
// find argument-expression of a function
TokenArray getFuncOperand (TokenArray expr, int index) {
    if (isConstant(expr.array + index + 1)) return (TokenArray) {1, expr.array + index + 1};
    if (isOperator(expr.array + index + 1) && expr.array[index + 1].act == bro)
        return getOperand(expr, index + 1, 1, 1);
    printf("Error: No arguments passed2!\n");
    exit(EMPTY_BRACKETS);
}

// recursively build a calculation tree
// search for the last-evaluated operation of function and then
// call makeTree() function on each of its operands
TreeNode * makeTree(TokenArray expr) {
    // get the index of root element of a tree
    int rootIndex = indexOfRootOp(expr);
    TreeNode *outNode = malloc(sizeof(TreeNode));
    if (rootIndex < 0 || isDefinedConst(expr.array + rootIndex)) {
        // expression is a single constant or variable
        outNode->data = expr.array[0];
        outNode->left = outNode->right = NULL;
        return outNode;
    }
    // expression contains a function or operation
    outNode->data = expr.array[rootIndex];
    if (rootIndex == 0 || isFunc(&outNode->data) || isUnaryMinus(&outNode->data)) {
        // operation is first in expression or is unary minus and has only right operand
        outNode->left = NULL;
        outNode->right = makeTree(getFuncOperand(expr, rootIndex));
    } else {
        outNode->left = makeTree(getOperand(expr, rootIndex, 0, 0));
        outNode->right = makeTree(
                expr.array[rootIndex].act == car ? getFuncOperand(expr, rootIndex) : getOperand(expr, rootIndex, 1, 0));
    }
    if (isFunc(&outNode->data) && (outNode->data.act == log || outNode->data.act == pov)) {
        TreeNode *child = outNode->right;
        if (!(isOperator(&child->data) && child->data.act == cma)) {
            printf("Error: wrong log() or pow() format!\n");
            exit(WRONG_LOG);
        }
        outNode->right = child->right, outNode->left = child->left;
        free(child);
    }
    return outNode;
}

// recursively calculate single tree of expression
complex double calcExpr(TreeNode * tree, complex double * variables, int * initialized) {
    if (!tree) return 0;
    // if current node present constant return its value without recursion
    if (isConstant(&tree->data)) return tree->data.value * (tree->data.imag ? I : 1);
    //  for return if PI or E
    if (isDefinedConst(&tree->data)) return funcArray[tree->data.act](0, 0);
    // return value of variable
    if (isVariable(&tree->data)) return variables[tree->data.varID];

    // recursively calculate left subtree and right subtree
    complex double b = calcExpr(tree->right, variables, initialized),
                   a = calcExpr(tree->left, variables, initialized);

    // checking for usage of variable without assigning them
    if (tree->left && isVariable(&tree->left->data) && tree->data.act != eqv && !initialized[tree->left->data.varID] ||
        tree->right && isVariable(&tree->right->data) && !initialized[tree->right->data.varID]) {
        printf("Error! using variable before assignment\n");
        exit(NOT_ASSIGNED);
    }
    // asserting left side of '=' is a variable
    if (tree->data.act == eqv) {
        if (!isVariable(&tree->left->data)) {
            printf("Error! left part of assigning has to be variable\n");
            exit(CANNOT_ASSIGN);
        }
        initialized[tree->left->data.varID] = 1;
        return  variables[tree->left->data.varID] = b;
    }
    // perform action and return result
    return funcArray[tree->data.act](a, b);
}

// calculate whole entered expression by building
complex double calculate(ParsedExpression pe) {
    // allocate memory for all variable in given expression
    complex double variables[pe.varCount];
    // checking array if variable assigned or not
    int initialized[pe.varCount];
    complex double result;
    for (int i = 0; i < pe.varCount; ++i) initialized[i] = 0;
    for (int i = pe.lineCount - 1; i >= 0; --i) {
        TreeNode *tree = makeTree(pe.lines[i]);
        printTree(tree);      // debug
        printf("\n");         // debug
        result = calcExpr(tree, variables, initialized);
        printResult(result);  // debug
        printf("\n");         // debug
    }
    return result;
}
#endif