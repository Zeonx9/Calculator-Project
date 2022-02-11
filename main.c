#include <stdio.h>
#include <complex.h>
#include "input_parse.h"
#include "calc_tree.h"
#include "tests_print.h"

int main() {
    ParsedExpression pe = parseExpression();
    complex double a = calculate(pe);
    deletePe(pe);
    printf("\nresult: "); printResult(a);
}