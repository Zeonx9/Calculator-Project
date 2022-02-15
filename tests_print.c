#include "tests_print.h"
#include "func_wraps.h"

void printToken(Token * t) {
    if (t->type == constant) printf("%.2lf%s", t->value, t->imag ? "i" : "");
    else if (t->type == operation || (t->type == identifier && t->act != none)) printf("%s", RESERVED[t->act]);
    else printf("var(id:%d)", t->varID);
}

void printTree(TreeNode * tn) {
    if (!tn) { printf("null"); return;}
    printToken(tn->data);
    if (tn->right || tn->left) {
        printf(" [");
        printTree(tn->left);
        printf("; ");
        printTree(tn->right);
        printf("]");
    }
}
void printResult(complex double r) {
    if (isZero(cimag(r))) {
        printf("%g", creal(r));
    } else if (isZero(creal(r))) {
        printf("%gj", cimag(r));
    } else printf("%g%+gj", creal(r), cimag(r));
}