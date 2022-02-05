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
        TreeNode *tree = buildTree(handleBrackets(pe.lines[i]));
        printTree(tree);      // debug
        printf("\n");         // debug
        result = calcExpr(tree, variables, initialized);
        printResult(result);  // debug
        printf("\n");         // debug
    }
    return result;
}

Expression handleBrackets(TokenArray expr) {
    Expression out = {0, malloc(expr.size * sizeof(ExprNode))};
    int i = 0;
    for (Token * cur = expr.self; i < expr.size; ++cur, ++i) {
        ExprNode node;
        if (!(isOperator(cur) && cur->act == bro)) {
            node.isToken = 1, node.token = cur;
        } else {
            int k = 0, depth = 0;
            for (Token * end = cur; k == 0 || depth != 0; ++k, ++end) {
                if (isOperator(end)) {
                    if (end->act == bro) ++depth;
                    if (end->act == brc) --depth;
                }
            }
            node.isToken = 0;
            Expression bracketsOff = handleBrackets((TokenArray) {k - 2, cur + 1});
            node.expr.size = bracketsOff.size, node.expr.self = bracketsOff.self;
            cur += k - 1, i += k - 1;
        }
        out.self[out.size++] = node;
    }
    return out;
}

int indexOfRootOperation(Expression expr) {
    int ind = -1, curOpDepth = 0, curOpPriority = 0, depth = 0;
    // for each token if it is operator check its priority and bracket-depth
    for (ExprNode *cur = expr.self; cur < expr.self + expr.size; ++cur)
        if (cur->isToken) {
            if (priority(cur->token) < 0) continue;
            // if no operator found or current operator is lower depth or same but lower priority then update
            if (!isFinal(cur->token) &&
                    (ind < 0 || depth < curOpDepth || curOpDepth == depth && priority(cur->token) <= curOpPriority)) {
                ind = (int) (cur - expr.self);
                curOpDepth = depth;
                curOpPriority = priority(cur->token);
            }
        }
    return ind;
}

TreeNode * buildTree(Expression expr) {
    if (!expr.size) {
        printf("Error: No arguments passed2!\n");
        exit(EMPTY_BRACKETS);
    }
    int ind = indexOfRootOperation(expr);
    if (ind < 0 && !expr.self[0].isToken)
            return buildTree((Expression) {expr.self[0].expr.size, expr.self[0].expr.self});
    TreeNode *tree = malloc(sizeof(TreeNode));
    tree->data = *expr.self[ind < 0 ? 0 : ind].token;
    if (ind < 0 || isDefinedConst(&tree->data)) {
        tree->left = tree->right = NULL;
        return tree;
    }
    tree->right = buildTree((Expression) {expr.size - ind - 1, expr.self + ind + 1});
    tree->left = ind == 0 ? NULL :  buildTree((Expression) {ind, expr.self});
    return tree;
}
#endif