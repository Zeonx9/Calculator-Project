#define T
#include "input_parse.h"

#ifdef T
#define MAX_EXPR_LEN 300 // максимальная длинна строки
const char fileName[] = "saved_lines_file.txt"; // имя файла куда сохраняется выражение
#include <string.h>
#include <stdlib.h>
#include "func_wraps.h"
#include "error_handle.h"
/** реализация массва зарезервированных идентификаторов, если такое слово будет встречено в строке,
  * оно будет интерпреитоваться как оператор,  функция или математическая константа **/
const char * RESERVED[NUM_OF_RESERVED] = {
        "+", "-", "*", "/", "^", "=", "(", ")", ",", // операторы
        "sin", "cos", "tg", "log", "ln", "sqrt", "pow", "abs", "exp", "real", "imag", "mag", "phase", // функции
        "e", "PI"}; // математические константы

/// возвращает код встреченого символа, если он представляет собой оператор иначе возвращает код none
Function getOpCode(char c) {
    for (int i = 0; i < NUM_OF_OPERATIONS; ++i)
        if (*RESERVED[i] == c) return (Function) i;
    return none;
}

///  возвращает код встреченого слова, если строка не является зарезервированным словом, то возвращается none
Function getFuncCode(const char *s) {
    for (int i = NUM_OF_OPERATIONS; i < NUM_OF_RESERVED; ++i)
        if (strcmp(s, RESERVED[i]) == 0) return (Function) i;
    return none;
}

/** находит переменную по имени в пуле переменных и возвращает ее индекс,
 *  если совпадений нет, то добанляет встреченное имя в конец пула и возвращает индекс **/
int getVariableID(const char *name, char **pool, int *len) {
    for (int i = 0; i < *len; ++i)
        if (strcmp(pool[i], name) == 0) return i;
    strcpy(pool[(*len)++], name);
    return *len - 1;
}

/// пропускает в строке пробелы или символы новой строки
void skip(char ** p) {
    while (**p == ' ' || **p == '\n') (*p)++;
}

/** проверка на то, что встреченный символ являеся цифрой, десятичной точкой или 'j',
  * только эти символы могут входить в и начинать числовую константу **/
int isDigitOrJ(char c) {
    return '0' <= c && c <= '9' || c == '.' || c == 'j';
}
/** в идентификаторах разрашены буквы любого регистра и цифры ( не лидирующие )
  * метод проверяет переданный символ и возращает резултьтат проверки **/
int isAllowedInId(char c) {
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9';
}

/// алгоритм проверки правильности скобочной последовательности, линейная проверка всего выражения
int checkBracketSequence(TokenArray expr) {
    int depth = 0;
    for (int i = 0; i < expr.size; ++i)
        if (expr.self[i].type == operation) {
            if (expr.self[i].act == bro) ++depth;
            if (expr.self[i].act == brc) --depth;
            if (depth < 0) return 0;
        }
    return depth == 0;
}

/// линейная проверка выражения, что нет подряд идущих числовых констант или переменных
int checkNoFollowingConstants(TokenArray expr) {
    for (int i = 0; i < expr.size - 1; ++i) {
        if (isFinal(expr.self + i) && isFinal(expr.self + i + 1))
            return 0;
    }
    return 1;
}

// реализации соответствущих методов проверки токенов
int isConstant(Token *t) { return t->type == constant; }
int isFunc(Token *t) { return t->type == identifier && t->act != none; }
int isOperator(Token *t) { return t->type == operation; }
int isVariable(Token *t) { return t->type == identifier && t->act == none; }
int isFinal(Token *t) { return isConstant(t) || isVariable(t); }

/** главный метод парсинга стороки, по началу каждого "слова" определяет тип токена,
  * который будет это слово представлять, принимает на вход строку,
  * а также пул переменных и указатель на количество переменных
  * возвращает массив токенов, представляющий данное выражение **/
TokenArray tokenize (char *expr, char ** variablesPool, int *varCount) {
    int size = 0, bufCount = 0, strLen = (int) strlen(expr), tmp;
    Token * tokens = malloc(strLen * sizeof(Token));
    char buf[NUM_OF_RESERVED] = "", *pos = expr;

    skip(&pos);
    while (*pos) {
        // t это указатель на текущий токен в его поля будет записана информация о следущем встреченном элементе
        Token *t = tokens + size++;
        // переменные для хранения рвеменных значений полей для токена
        TokenType type; double value; Function actCode; int isImaginary, varId;

        // проверка первого символа элемента, на то, что он может встетиться в правильном выражении
        if (!isDigitOrJ(*pos) && !isAllowedInId(*pos) && *pos != ' ' && *pos != ',' && *pos == '\n')
            errorLog(ILLEGAL_CHAR);

        if (isDigitOrJ(*pos)) {
            // если слово началось с цифры, точки или j то это слово является численной константой
            type = constant;
            // cчитать все слово в буфер
            while (*pos && isDigitOrJ(*pos))
                buf[bufCount++] = *pos++;
            buf[bufCount] = 0;
            // определить соответствующие поля
            value = bufCount == 1 && *buf == 'j' ? 1 : atof(buf); // NOLINT(cert-err34-c)
            isImaginary = buf[bufCount - 1] == 'j';
        } else if ((tmp = getOpCode(*pos)) != none) {
            // символ это оператор
            type = operation;
            actCode = tmp; ++pos;
        } else {
            // иначе это либо переменная либо функция
            type = identifier;
            // считать все в буфер
            while (*pos && isAllowedInId(*pos))
                buf[bufCount++] = *pos++;
            buf[bufCount] = 0;
            // если слово зарезервированно - это функция
            if ((tmp = getFuncCode(buf)) != none) {
                varId = -1;
                if (tmp == PI || tmp == E) {
                    type = constant;
                    value = funcArray[tmp](0, 0);
                }
            }
            else // иначе переменная
                varId = getVariableID(buf, variablesPool, varCount);
            actCode = tmp;
        }

        // установить токену нужные значения полей
        t->type = type;
        if (type == constant)
            // поля числовых констант
            t->value = value, t->imag = isImaginary;
        else
            // поля для операторов и идентификоторов
            t->act = actCode, t->varID = varId;
        bufCount = 0;
        skip(&pos);
    }
    return (TokenArray) {size, tokens};
}

/** метод считывающий ввод с консоли и возвращиющий объект типа InputExpression
*   ввод прекращается если введена путстая строка **/
InputExpression getInput() {
    InputExpression ie = {};
    char buffer[MAX_EXPR_LEN];
    ie.savedFile = fopen(fileName, "w+"); // файл открыт для записи и чтения
    // считывать строки, пока не встретится пустая
    while (*fgets(buffer, MAX_EXPR_LEN, stdin) != '\n') {
        ie.lineCount++;
        fputs(buffer, ie.savedFile); // сохранить строку в файл
    }
    // установить указатьель файла в начало, для последующего чтения
    fseek(ie.savedFile, 0, SEEK_SET);
    ie.bufferSize = MAX_VARS_IN_LINE * ie.lineCount;
    return ie;
}

// парсинг всего введенного выражения с применением метода tokenize() для каждой строки
ParsedExpression parseExpression() {
    InputExpression ie = getInput();
    TokenArray * output = malloc(ie.lineCount * sizeof(TokenArray));
    // создания пула переменных
    int varCount = 0;
    char **variablesPool = malloc(ie.bufferSize * sizeof (char *));
    for (int i = 0; i < ie.bufferSize; ++i)
        variablesPool[i] = malloc(LEN_OF_IDENTIFIER * sizeof(char));

    char curExpr[MAX_EXPR_LEN]; // буфер для текужей строки
    for (int i = 0; i < ie.lineCount; ++i) {
        // считать строку и использовать tokenize
        fgets(curExpr, MAX_EXPR_LEN, ie.savedFile);
        output[i] = tokenize(curExpr, variablesPool, &varCount);
        // проверка на правильность скобок
        if (!checkBracketSequence(output[i]))
            errorLog(INCORRECT_BRACKETS);
        // проверка, что нет подряд идущих констант или переменных
        if (!checkNoFollowingConstants(output[i]))
            errorLog(FOLLOWING_CONSTANTS);
    }

    // освободить ресурсы использованные под пул и закрыть файл
    for (int i = 0; i < ie.bufferSize; ++i)
        free(variablesPool[i]);
    free(variablesPool);
    fclose(ie.savedFile);

    return (ParsedExpression) {output, ie.lineCount, varCount};
}

// освобождение ресурсов
void deletePe(ParsedExpression pe) {
    for (int i = 0; i < pe.lineCount; ++i)
        free(pe.lines[i].self);
    free(pe.lines);
}
#endif
