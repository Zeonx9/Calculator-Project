#ifndef CPROJECTSCALCULATOR_INPUT_PARSE_H
#define CPROJECTSCALCULATOR_INPUT_PARSE_H

#include <stdio.h>

#define NUM_OF_RESERVED 24 // number of reserved identifiers
#define NUM_OF_OPERATIONS 9 // number of operations + brackets + '=' + ','
#define LEN_OF_IDENTIFIER 32 // maximal len of identifier

#define INCORRECT_BRACKETS 101
#define ILLEGAL_CHAR 102
#define EMPTY_BRACKETS 103
#define FOLLOWING_CONSTANTS 104
#define WRONG_LOG 105
#define CANNOT_ASSIGN 106
#define NOT_ASSIGNED 107

extern const char * RESERVED[NUM_OF_RESERVED];

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
    Token * self;
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

// check if token is a constant
int isConstant(Token *t);
// check if token is a variable
int isVariable(Token *t);
// check if given operation is a defined function
int isFunc(Token *t);
// check if token is operator
int isOperator(Token *t);
// check if given token is constant or variable
int isFinal(Token *t);
// check if given operation is a defined constant: 'PI' or 'e'
int isDefinedConst(Token *t);


#endif //CPROJECTSCALCULATOR_INPUT_PARSE_H
