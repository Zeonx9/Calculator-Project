#include <stdio.h>
#include <complex.h>
#include "input_parse.h"
#include "calc_tree.h"
#include "tests_print.h"

int main() {
    ParsedExpression pe = parseExpression();
    for (int i = 0; i < pe.lineCount; ++i) {
        TokenArray ta = pe.lines[i];
        for (Token *t = ta.self; t < ta.self + ta.size; ++t)
            printToken(t), printf(" ");
        printf("\n");
    }
    complex double a = calculate(pe);
    deletePe(pe);
    printf("\nresult: "); printResult(a);
    printf("\nvariables: %d, lines: %d", pe.varCount, pe.lineCount);
}