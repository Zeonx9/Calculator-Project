#include "tests_print.h"

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
    if (!cimag(r)) {
        printf("%lf", creal(r));
    } else if (!creal(r)) {
        printf("%lfi", cimag(r));
    } else printf("%lf+%lfi", creal(r), cimag(r));
}