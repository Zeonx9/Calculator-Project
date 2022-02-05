#ifndef CPROJECTSCALCULATOR_CALC_TREE_H
#define CPROJECTSCALCULATOR_CALC_TREE_H

#include "input_parse.h"
#include <complex.h>

/** рекурсивная структура описывающая дерево
 *  каждый узел имеет поле data типа Token, которое содержит информацию об узле
 *  также ссылки на левое и правое поддерево left и right соответствено типа TreeNode *
 *  если у узла нет какого либо поддерева или обоих, то записывается значение NULL **/
typedef struct tree_node {
    Token * data;
    struct tree_node * left;
    struct tree_node * right;
} TreeNode;

/** струтура узел выражения представляет либо токен либо массив токенов
 *  используется для работы со скобками, если выражение содержит скобки,
 *  то подвыражение в скобках будет выделено в отдельное выражение
 *  определяет что конктерно лежит в узле в данный момент поле isToken**/
typedef struct node_expr{
    int isToken;
    union {
        Token * token;
        struct {
            int size;
            struct node_expr * self;
        } expr;
    };
} ExprNode;

/** структура выражения представляет массив "узлов выражения" **/
typedef struct {
    int size;
    ExprNode * self;
} Expression;

/// метод находит скобки в выражении и рекурсивно выделяет все в них заключенное в одно выражение
Expression handleBrackets(TokenArray expr);

/** рекурсивный метод построения дерева вычислений, находится оператор выполняющийся последним,
 * затем для его операндов рекурсивно вычисляются поддеревья.
 * константы и переменные являются листьями и не имеют поддеревьев **/
TreeNode * buildTree(Expression expr);

/** по данному дереву вычисляет его значение принимает дерево,
 * массив куда будут записаны значения для переменных, а также массив флагов инициализации,
 * возвращает результат вычислений **/
complex double calcExpr(TreeNode * tree, complex double * variables, int * initialized);

/// вычисляет все введенное выражение построчто строя дерево и вычисляя его
complex double calculate(ParsedExpression pe);
#endif //CPROJECTSCALCULATOR_CALC_TREE_H
