#define ACTIVE_

#ifdef ACTIVE
#include <stdio.h>
#include "input_parse.h"

void printToken(Token * t) {
    if (t->type == constant) printf("%.2lf%s", t->value, t->imag ? "j" : "");
    else if (t->type == operation || (t->type == identifier && t->act != none)) printf("%s", RESERVED[t->act]);
    else printf("var(id:%d)", t->varID);
}

int main() {
    InputExpression ie = getInput();
    ParsedExpression pe = parseExpression(ie);
    for (int i = 0; i < ie.lineCount; ++i) {
        TokenArray ta = pe.lines[i];
        for (Token *t = ta.array; t < ta.array + ta.size; ++t)
            printToken(t);
        printf("\n");
    }
    printf("variables: %d, lines: %d", pe.varCount, pe.lineCount);
}
#endif
