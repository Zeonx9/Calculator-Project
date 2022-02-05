#include <stdio.h>
#include <complex.h>
#include "input_parse.h"
#include "calc_tree.h"
#include "tests_print.h"

int main() {
    InputExpression ie = getInput();
    ParsedExpression pe = parseExpression(ie);
    for (int i = 0; i < ie.lineCount; ++i) {
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