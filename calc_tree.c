#include <malloc.h>
#include <assert.h>
#include <complex.h>
#include "calc_tree.h"
#include "func_wraps.h"
#include "tests_print.h"

/** приоритеты операций в убывающем порядке
  * функции > ^ > * и , > + и - > , > = **/
int priority(Token * t) {    if (t->type == identifier && t->act != none) return 6;
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

// рекурсивно вычисляет выражение
complex double calcExpr(TreeNode * tree, complex double * variables, int * initialized) {
    if (!tree) return 0;
    // если узел содержит константу то вернуть ее значение
    if (isConstant(tree->data)) return tree->data->value * (tree->data->imag ? I : 1);
    // вернуть значение переменной
    if (isVariable(tree->data)) return variables[tree->data->varID];

    // рекурсивные вызовы подсчета операндов
    complex double b = calcExpr(tree->right, variables, initialized),
                   a = calcExpr(tree->left, variables, initialized);

    // проверка, что все используемые переменные инициализированы
    if (tree->left && isVariable(tree->left->data) && tree->data->act != eqv && !initialized[tree->left->data->varID] ||
        tree->right && isVariable(tree->right->data) && !initialized[tree->right->data->varID])
        errorLog(NOT_ASSIGNED);
    // проверка, что слева от равно стоит переменная
    if (tree->data->act == eqv) {
        if (!isVariable(tree->left->data))
            errorLog(CANNOT_ASSIGN);
        initialized[tree->left->data->varID] = 1;
        return  variables[tree->left->data->varID] = b;
    }
    // произвести действие над операндами, вернуть результат
    return funcArray[tree->data->act](a, b);
}

/// очистить занятую под дерево память
void deleteTree(TreeNode * tree) {
    if (!tree) return;
    deleteTree(tree->left);
    deleteTree(tree->right);
    free(tree);
}

/// очистить занятую под выражение память
void deleteExpression(Expression e) {
    for (int i = 0; i < e.size; ++i)
        if (!e.self[i].isToken)
            deleteExpression((Expression) {e.self[i].expr.size, e.self[i].expr.self});
    free(e.self);
}

// посчитать все выражение построив для каждой строки дерево и посчитав
complex double calculate(ParsedExpression pe) {
    // выделить память под переменные исползуемые в выражении
    complex double variables[pe.varCount];
    // создать массив флагов инициализации
    int initialized[pe.varCount];
    complex double result;
    for (int i = 0; i < pe.varCount; ++i) initialized[i] = 0;
    for (int i = pe.lineCount - 1; i >= 0; --i) {
        // построить дерево
        Expression expr = handleBrackets(pe.lines[i]);
        TreeNode *tree = buildTree(expr);
        deleteExpression(expr);
        printTree(tree);      // debug
        printf("\n");         // debug
        // вычислить
        result = calcExpr(tree, variables, initialized);
        deleteTree(tree);
        printResult(result);  // debug
        printf("\n");         // debug
    }
    return result;
}

// обработка скобок
Expression handleBrackets(TokenArray expr) {
    Expression out = {0, malloc(expr.size * sizeof(ExprNode))};
    int i = 0;
    for (Token * cur = expr.self; i < expr.size; ++cur, ++i) {
        ExprNode node;
        if (!(isOperator(cur) && cur->act == bro)) {
            // если не встречена открывающая скобка, то узел соответствует токену
            node.isToken = 1, node.token = cur;
        } else {
            // для встреченной скобки ищется парная, все выражение внутри выделяется
            int k = 0, depth = 0;
            for (Token * end = cur; k == 0 || depth != 0; ++k, ++end) {
                if (isOperator(end)) {
                    if (end->act == bro) ++depth;
                    if (end->act == brc) --depth;
                }
            }
            node.isToken = 0;
            // рекурсивная проверка на скобаки и присвоение текущему узлу всего выделенного выражения
            Expression bracketsOff = handleBrackets((TokenArray) {k - 2, cur + 1});
            node.expr.size = bracketsOff.size, node.expr.self = bracketsOff.self;
            cur += k - 1, i += k - 1;
        }
        out.self[out.size++] = node;
    }
    return out;
}

/** поиск последнего выполняющегося оператора, который будет корнем дерева
  * возвращает индекс найденого элемента или -1 если не найдено **/
int indexOfRootOperation(Expression expr) {
    int ind = -1, curOpPriority = 0;
    // проверить для каждого узла если он токен и может иметь поддеревья
    for (ExprNode *cur = expr.self; cur < expr.self + expr.size; ++cur)
        if (cur->isToken) {
            // пропустить все токены без действия
            if (priority(cur->token) < 0) continue;
            // обновить оператор если его приоритет ниже или он стоит левее
            if (ind < 0 || priority(cur->token) <= curOpPriority) {
                ind = (int) (cur - expr.self);
                curOpPriority = priority(cur->token);
            }
        }
    return ind;
}

// построение дерева
TreeNode * buildTree(Expression expr) {
    // проверить, что выражение не пусто
    if (!expr.size)
        errorLog(EMPTY_BRACKETS);
    // найти коренную операцию
    int ind = indexOfRootOperation(expr);
    // если нет операции на данном уровне вложенности и первый узел не токен,
    // то рекурсивно построить дерево от выражения узла
    if (ind < 0 && !expr.self[0].isToken)
            return buildTree((Expression) {expr.self[0].expr.size, expr.self[0].expr.self});
    // выделить память под узел и присвоить информацию
    TreeNode *tree = malloc(sizeof(TreeNode));
    tree->data = expr.self[ind < 0 ? 0 : ind].token;
    if (ind < 0) {
        tree->left = tree->right = NULL;
        return tree;
    }
    // рекурсивно опрделить поддеревья
    tree->right = buildTree((Expression) {expr.size - ind - 1, expr.self + ind + 1});
    tree->left = ind == 0 ? NULL :  buildTree((Expression) {ind, expr.self});
    // проверка, что у функций логарифма и возведения в степень два аргумента перечисленных через запятую
    if (isFunc(tree->data) && (tree->data->act == log || tree->data->act == pov)) {
        TreeNode *child = tree->right;
        if (!(isOperator(child->data) && child->data->act == cma))
            errorLog(WRONG_LOG_POW);
        tree->right = child->right, tree->left = child->left;
        free(child);
    }
    return tree;
}