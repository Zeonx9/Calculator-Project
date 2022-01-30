#ifndef INC_22_02_15_CALCULATOR_INPUT_PARSE_H
#define INC_22_02_15_CALCULATOR_INPUT_PARSE_H

#include <stdio.h>

#define NUM_OF_RESERVED 24 // number of reserved identifiers
#define NUM_OF_OPERATIONS 9 // number of operations + brackets
#define LEN_OF_IDENTIFIER 32 // maximal len of identifier

const char RESERVED[NUM_OF_RESERVED][6];

typedef enum { identifier, operation, constant } TokenType; // used to determine behaviour of elements
typedef enum { // represent functions as a int code
    add, sub, mul, dev, car, eqv, bro, brc, cma,
    sin, cos, tg, log, ln, srt, pov, abc, exp, real, img, mag, phs,
    E, PI, none } Function;

typedef struct { // represent single element in expression string
    TokenType type;
    union {
        struct { double value; int imag; };
        struct { Function act; int varID; };
    };
} Token;

typedef struct {
    int size;
    Token * array;
} TokenArray;

typedef struct {
    int lineCount;
    FILE *savedFile;
    int bufferSize;
} InputExpression;

typedef struct {
    TokenArray * lines;
    int lineCount;
    int varCount;
} ParsedExpression;

InputExpression getInput(); // prepare input for parsing
ParsedExpression parseExpression(InputExpression ie); // parse whole given expressions

#endif //INC_22_02_15_CALCULATOR_INPUT_PARSE_H
