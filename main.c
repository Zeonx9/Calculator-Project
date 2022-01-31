#define ACTIVE_

#ifdef ACTIVE
#include <stdio.h>
#include "input_parse.h"

int main() {
    InputExpression ie = getInput();
    ParsedExpression pe = parseExpression(ie);
    for (int i = 0; i < ie.lineCount; ++i) {
        TokenArray ta = pe.lines[i];
        for (Token *t = ta.array; t < ta.array + ta.size; ++t)
            printf("%d ", t->type);
        printf("\n");
    }
    printf("variables: %d, lines: %d", pe.varCount, pe.lineCount);
}
#endif
