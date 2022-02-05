#ifndef CPROJECTSCALCULATOR_INPUT_PARSE_H
#define CPROJECTSCALCULATOR_INPUT_PARSE_H

#include <stdio.h>

// численные константы
#define NUM_OF_RESERVED 24 // число зарезервированных идентификаторов под функции и математические константы
#define NUM_OF_OPERATIONS 9 // число операторов: арифметических операции, скобки, равно и запятая
#define LEN_OF_IDENTIFIER 32 // максимальная длинна идентификатора

// коды ошибок
#define INCORRECT_BRACKETS 101 // если выражение содержит неправильную скобочную последовательность
#define ILLEGAL_CHAR 102 // встречен символ, который не может находиться внутри арифметического выражения
#define EMPTY_BRACKETS 103 // переданы скобки не содержащие чего-либо
#define FOLLOWING_CONSTANTS 104 // две переменные или константы идут последовательно без оператора между
#define WRONG_LOG_POW 105 // внутри логарифма или функции возведения в степень нет запятой
#define CANNOT_ASSIGN 106 // слева от равно стоит не переменная
#define NOT_ASSIGNED 107 // использование переменной до ее объявления и присваивания ей значения

// массив содержащий все зарезервированные слова, а также символы операторов
extern const char * RESERVED[NUM_OF_RESERVED];

// программные константы типа токена: идентификатор, оператор или числовая константа
// записаны в поле type каждого токена и определяют поведение токена
typedef enum { identifier, operation, constant } TokenType;

// программные константы дейсвия токена, если токен представляет функцию или оператор,
// то в поле act токена стоит соответствующая константа, иначе константа none указывющая на то, что у токена нет
// действия примером таких токенов являются переменные
typedef enum {
    add, sub, mul, dev, car, eqv, bro, brc, cma,
    sin, cos, tg, log, ln, srt, pov, abc, exp, real, img, mag, phs,
    E, PI, none } Function;

/* ключевая структура парсинга, каждый отделный элемент входного выражения представлен именно этим типом
 * поле type определяет тип токена, возможные типы представленны перечисление TokenType
 * далее если type == constant, to токен имеет два поля value - числовое значение и imag - флаг о том, что число мнимое
 * если type == identifier то токен представляет собой функцию или переменую и имеет два поля varID - код переменной
 * и act - код действия функции из перечислени Function, none для переменных
 * и если type == operation, то для токена также определены varID и act, но используется только act */
typedef struct {
    TokenType type;
    union {
        struct { double value; int imag; };
        struct { Function act; int varID; };
    };
} Token;

// массив из токенов, содержит указатель на начало массива и количесво элементов в нем
typedef struct {
    int size;
    Token * self;
} TokenArray;

// сырой тип введеных данных, используется, чтобы подсчитать сколько максимально байт понадобится выделить для
// хранения возможных переменны, подсчитывается количество строк, все считанное с консоли выражение сохраняется в файл
typedef struct {
    int lineCount; // количество строк в выражении
    FILE *savedFile; // файл с сохраненными данными
    int bufferSize; // суммарная длинна ввех строк в выражении
} InputExpression;

// итоговый тип выражения после парсинга, каждая строка представлена массивом токенов, поэтому структура хранит
// указатель lines на массив массивов токенов
// также подсчитано число различных переменных встречающихся в выражении, каждой переменной присвоен ее код из от 0 до
// varCount, этот код сохранен в поле varID для каждого токена представляющего перемнную
typedef struct {
    TokenArray * lines;
    int lineCount;
    int varCount;
} ParsedExpression;

// метод считывающий ввод с консоли и возвращиющий объект типа InputExpression
// ввод прекращается если введена путстая строка
InputExpression getInput();
// основной метод парсинга, принимает объект InputExpression и возвращает объект ParsedExpression
ParsedExpression parseExpression(InputExpression ie);

// булевые методы проверки токена на то, какая информация в нем хранится
int isConstant(Token *t); // являеся ли токен численной константой
int isVariable(Token *t); // является ли токен переменой
int isFunc(Token *t); // является ли токен функцией
int isOperator(Token *t); // является ли токен оператором (арифметическим, скобкой, равно или запятой)
int isFinal(Token *t); // проверка на то, что токен не имеет действия,
                       // то есть является переменной или численной константой
int isDefinedConst(Token *t); // является ли токен математической константой PI или e
#endif //CPROJECTSCALCULATOR_INPUT_PARSE_H
