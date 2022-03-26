#include <stdio.h>
#include "input_parse.h"
#include "tests_print.h"

int main() {
    ParsedExpression pe = parseExpression();
    complex double a = calculate(pe);
    deletePe(pe);
    printf("\nresult: "); printResult(a);
}