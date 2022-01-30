#ifndef INC_22_02_15_CALCULATOR_INPUT_PARSE_H
#define INC_22_02_15_CALCULATOR_INPUT_PARSE_H

#include <stdio.h>

#define NUM_OF_RESERVED 24 // number of reserved identifiers
#define NUM_OF_OPERATIONS 9 // number of operations + brackets
#define LEN_OF_IDENTIFIER 32 // maximal len of identifier

const char RESERVED[NUM_OF_RESERVED][6];

// constants to distinct different types of possible elements of expression
typedef enum { identifier, operation, constant } TokenType;
// constants for every reserved function identifier and operators
// actually reserved words are in .c file in the same order as here
typedef enum {
    add, sub, mul, dev, car, eqv, bro, brc, cma,
    sin, cos, tg, log, ln, srt, pov, abc, exp, real, img, mag, phs,
    E, PI, none } Function;


// core struct which represent single element in expression string
// it can be either number(constant), operation(action) or identifier(function/variable)
// number should have value and flag of it being imaginary
// operation and functions should have id of its acton
// each of variables get a code [0 - number of vars)
typedef struct {
    TokenType type;
    union {
        struct { double value; int imag; };
        struct { Function act; int varID; };
    };
} Token;

// array of tokens: pointer to start and size of it
typedef struct {
    int size;
    Token * array;
} TokenArray;

// raw input and metadata for parsing
typedef struct {
    int lineCount;
    FILE *savedFile; // file with got input saved
    int bufferSize; // sum of all lines length
} InputExpression;

// output struct contains every line parsed, number of lines and how much different variable used
typedef struct {
    TokenArray * lines;
    int lineCount;
    int varCount;
} ParsedExpression;

// prepare input for parsing
InputExpression getInput();
// parse whole given expressions
ParsedExpression parseExpression(InputExpression ie);

#endif //INC_22_02_15_CALCULATOR_INPUT_PARSE_H
