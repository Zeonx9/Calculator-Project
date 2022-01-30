#define T
#include "input_parse.h"

#ifdef T
#define MAX_EXPR_LEN 300
#include <string.h>
#include <stdlib.h>

const char RESERVED[NUM_OF_RESERVED][6] = {  //  list of reserved identifiers
        "+", "-", "*", "/", "^", "=", "(", ")", ",",
        "sin", "cos", "tg", "log", "ln", "sqrt", "pow", "abs", "exp", "real", "imag", "mag", "phase",
        "e", "PI"};

Function getOpCode(char c) { // return a code of given operator if it is else a 'none' code
    for (int i = 0; i < NUM_OF_OPERATIONS; ++i)
        if (*RESERVED[i] == c) return (Function) i;
    return none;
}

Function getFuncCode(const char *s) { // return a code of given function if it is else a 'none' code
    for (int i = NUM_OF_OPERATIONS; i < NUM_OF_RESERVED; ++i)
        if (strcmp(s, RESERVED[i]) == 0) return (Function) i;
    return none;
}

int getVariableID(const char *name, char **pool, int *len) {
    for (int i = 0; i < *len; ++i)
        if (strcmp(pool[i], name) == 0) return i;
    strcpy(pool[(*len)++], name);
    return *len - 1;
}

void skip(char ** p) { // skip all leading white spaces if any
    while (**p == ' ' || **p == '\n') (*p)++;
}

int isDigitOrJ(char c) { // check if given character can start a constant literal
    return '0' <= c && c <= '9' || c == '.' || c == 'j';
}
int isAllowedInId(char c) { // only letters and numbers (not leading) allowed in identifiers
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9';
}

TokenArray tokenize (char *expr, char ** variablesPool, int *varCount) { // split given string (single line) into array of tokens
    int size = 0, bufCount = 0, tmp, strLen = (int) strlen(expr); // counters and temporary
    Token * tokens = malloc(strLen * sizeof(Token)); // resulting array of tokens
    char buf[NUM_OF_RESERVED] = "", *pos = expr; // buffer for a current single token

    skip(&pos);
    while (*pos) {
        Token *t = tokens + size++;
        TokenType type; double val; Function f; int imag, var; // values of fields of current token

        if (isDigitOrJ(*pos)) {
            type = constant; // if current token is a constant literal
            while (*pos && isDigitOrJ(*pos)) // get whole literal into buffer
                buf[bufCount++] = *pos++;
            buf[bufCount] = 0;

            val = bufCount > 1 ? atof(buf) : 1; // get the value
            imag = buf[bufCount - 1] == 'j'; // check if imaginary

        } else if ((tmp = getOpCode(*pos)) != none) {
            type = operation; // if current token is operator
            f = tmp; // get code of it
            ++pos;
        } else {
            type = identifier; // else it is identifier function or variable
            while (*pos && isAllowedInId(*pos)) // get whole literal into buffer
                buf[bufCount++] = *pos++;
            buf[bufCount] = 0;

            if ((tmp = getFuncCode(buf)) != none) { // if identifier is in reversed group it is function
                f = tmp;
                var = -1;
            } else { // else it is a variable
                var = getVariableID(buf, variablesPool, varCount);
                f = tmp;
            }
        }

        t->type = type; // assign current token its fields values
        if (type == constant) { // fields for constants
            t->value = val; t->imag = imag;
        } else { // fields for non-constants
            t->act = f;  t->varID = var;
        }
        bufCount = 0;
        skip(&pos);
    }
    return (TokenArray) {size, tokens};
}

InputExpression getInput() {
    InputExpression ie = {};
    char buffer[MAX_EXPR_LEN];
    char fileName[] = "saved_lines_file.txt";
    ie.savedFile = fopen(fileName, "w+");
    while (*fgets(buffer, MAX_EXPR_LEN, stdin) != '\n') {
        ie.bufferSize += (int) strlen(buffer);
        ie.lineCount++;
        fputs(buffer, ie.savedFile);
    }
    fseek(ie.savedFile, 0, SEEK_SET);
    return ie;
}

ParsedExpression parseExpression(InputExpression ie) {
    TokenArray * output = malloc(ie.lineCount * sizeof(TokenArray)); // resulting array
    int varCount = 0; // pool counter
    char **variablesPool = malloc(ie.bufferSize * sizeof (char *)); // pool of variable names
    for (int i = 0; i < ie.bufferSize; ++i)
        variablesPool[i] = malloc(LEN_OF_IDENTIFIER * sizeof(char));

    char curExpr[MAX_EXPR_LEN];
    for (int i = 0; i < ie.lineCount; ++i) { // tokenize every line given
        fgets(curExpr, MAX_EXPR_LEN, ie.savedFile);
        output[i] = tokenize(curExpr, variablesPool, &varCount);
    }

    //free all resources
    for (int i = 0; i < ie.bufferSize; ++i)
        free(variablesPool[i]);
    free(variablesPool);
    fclose(ie.savedFile);

    return (ParsedExpression) {output, ie.lineCount, varCount};
}

#endif
