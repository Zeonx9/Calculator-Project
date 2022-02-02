#define T
#include "input_parse.h"

#ifdef T
#define MAX_EXPR_LEN 300
const char fileName[] = "saved_lines_file.txt";
#include <string.h>
#include <stdlib.h>

// list of reserved identifiers
const char * RESERVED[NUM_OF_RESERVED] = {
        "+", "-", "*", "/", "^", "=", "(", ")", ",",
        "sin", "cos", "tg", "log", "ln", "sqrt", "pow", "abs", "exp", "real", "imag", "mag", "phase",
        "e", "PI"};

// return a code of given operator if it is else a 'none' code
Function getOpCode(char c) {
    for (int i = 0; i < NUM_OF_OPERATIONS; ++i)
        if (*RESERVED[i] == c) return (Function) i;
    return none;
}

// return a code of given function if it is else a 'none' code
Function getFuncCode(const char *s) {
    for (int i = NUM_OF_OPERATIONS; i < NUM_OF_RESERVED; ++i)
        if (strcmp(s, RESERVED[i]) == 0) return (Function) i;
    return none;
}

// search for existing variable if not found add new; return index
int getVariableID(const char *name, char **pool, int *len) {
    for (int i = 0; i < *len; ++i)
        if (strcmp(pool[i], name) == 0) return i;
    strcpy(pool[(*len)++], name);
    return *len - 1;
}

// skip all leading white spaces if any
void skip(char ** p) {
    while (**p == ' ' || **p == '\n') (*p)++;
}

// constants can contain only numbers, floating point and 'j'
// check if given character allowed in constant literal
int isDigitOrJ(char c) {
    return '0' <= c && c <= '9' || c == '.' || c == 'j';
}
// identifiers can contain letters and numbers (not leading)
// check if given character allowed in identifier
int isAllowedInId(char c) {
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9';
}

// check if entered expression present a correct bracket sequence
int checkBracketSequence(TokenArray expr) {
    int depth = 0;
    for (int i = 0; i < expr.size; ++i)
        if (expr.array[i].type == operation) {
            if (expr.array[i].act == bro) ++depth;
            if (expr.array[i].act == brc) --depth;
            if (depth < 0) return 0;
        }
    return depth == 0;
}

// check if between every constant or variable in expression is operation
int checkNoFollowingConstants(TokenArray expr) {
    for (int i = 0; i < expr.size - 1; ++i) {
        if (isFinal(expr.array + i) && isFinal(expr.array + i + 1))
            return 0;
    }
    return 1;
}

//check if given token is constant
int isConstant(Token *t) { return t->type == constant; }
// check if given operation is a defined function
int isFunc(Token *t) { return t->type == identifier && t->act != none; }
// check if token is operator
int isOperator(Token *t) { return t->type == operation; }
// check if token is variable
int isVariable(Token *t) { return t->type == identifier && t->act == none; }
// check if given token is constant or variable
int isFinal(Token *t) { return isConstant(t) || isVariable(t); }
// check if given operation is a defined constant 'PI' or 'e'
int isDefinedConst(Token *t) { return t->type == identifier && t->act == PI || t->act == E; }

// split given string (single line) into array of tokens
TokenArray tokenize (char *expr, char ** variablesPool, int *varCount) {
    // int variables: counters and temporary
    int size = 0, bufCount = 0, strLen = (int) strlen(expr), tmp;
    // resulting array of tokens
    Token * tokens = malloc(strLen * sizeof(Token));
    // buffer for a current single token
    char buf[NUM_OF_RESERVED] = "", *pos = expr;

    skip(&pos);
    while (*pos) {
        // pointer to current token and vars for values of its fields
        Token *t = tokens + size++;
        TokenType type; double value; Function actCode; int isImaginary, varId;

        if (!isDigitOrJ(*pos) && !isAllowedInId(*pos) && *pos != ' ' && *pos != ',' && *pos == '\n') {
            printf("Error: %c character is not allowed\n", *pos);
            exit(ILLEGAL_CHAR);
        }

        if (isDigitOrJ(*pos)) {
            // if current token is a constant literal
            type = constant;
            // get whole literal into buffer
            while (*pos && isDigitOrJ(*pos))
                buf[bufCount++] = *pos++;
            buf[bufCount] = 0;

            value = bufCount == 1 && *buf == 'j' ? 1 : atof(buf); // get the value NOLINT(cert-err34-c)
            isImaginary = buf[bufCount - 1] == 'j'; // check if imaginary
        } else if ((tmp = getOpCode(*pos)) != none) {
            // if current token is operator
            type = operation;
            actCode = tmp; ++pos;
        } else {
            // else it is identifier of function or variable
            type = identifier;
            // get whole literal into buffer
            while (*pos && isAllowedInId(*pos))
                buf[bufCount++] = *pos++;
            buf[bufCount] = 0;

            // if identifier is in reversed group it is function
            if ((tmp = getFuncCode(buf)) != none)
                varId = -1;
            else // else it is a variable
                varId = getVariableID(buf, variablesPool, varCount);
            actCode = tmp;
        }

        // assign current token its fields values
        t->type = type;
        if (type == constant)
            // fields for constants
            t->value = value, t->imag = isImaginary;
        else
            // fields for non-constants
            t->act = actCode, t->varID = varId;
        bufCount = 0;
        skip(&pos);
    }
    return (TokenArray) {size, tokens};
}

// get user input and prepare it for parsing
InputExpression getInput() {
    InputExpression ie = {};
    // buffer for line and a file where everything will be saved
    char buffer[MAX_EXPR_LEN];
    ie.savedFile = fopen(fileName, "w+");
    // process each given line if empty line entered then exit from loop
    while (*fgets(buffer, MAX_EXPR_LEN, stdin) != '\n') {
        ie.bufferSize += (int) strlen(buffer);
        ie.lineCount++;
        fputs(buffer, ie.savedFile);
    }
    // seek start of file for further reading
    fseek(ie.savedFile, 0, SEEK_SET);
    return ie;
}

// parse all given lines using tokenize method; return array of every tokenized line
ParsedExpression parseExpression(InputExpression ie) {
    // resulting array
    TokenArray * output = malloc(ie.lineCount * sizeof(TokenArray));
    // pool of variables actually is a set of strings
    int varCount = 0;
    char **variablesPool = malloc(ie.bufferSize * sizeof (char *));
    for (int i = 0; i < ie.bufferSize; ++i)
        variablesPool[i] = malloc(LEN_OF_IDENTIFIER * sizeof(char));

    char curExpr[MAX_EXPR_LEN]; // buffer for current string
    for (int i = 0; i < ie.lineCount; ++i) {
        // tokenize every line given and store into resulting array
        fgets(curExpr, MAX_EXPR_LEN, ie.savedFile);
        output[i] = tokenize(curExpr, variablesPool, &varCount);
        if (!checkBracketSequence(output[i])) {
            printf("Error: Entered expression has incorrect bracket sequence! line: %d\n", i + 1);
            exit(INCORRECT_BRACKETS);
        }
        if (!checkNoFollowingConstants(output[i])) {
            printf("Error: is not a single expression, two numbers or variables without operator!\n");
            exit(FOLLOWING_CONSTANTS);
        }
    }

    //free all used resources
    for (int i = 0; i < ie.bufferSize; ++i)
        free(variablesPool[i]);
    free(variablesPool);
    fclose(ie.savedFile);

    return (ParsedExpression) {output, ie.lineCount, varCount};
}

#endif
