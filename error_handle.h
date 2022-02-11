#ifndef CPROJECTSCALCULATOR_ERROR_HANDLE_H
#define CPROJECTSCALCULATOR_ERROR_HANDLE_H

/// все коды ошибок идут последовательно и начинаются с этого значения
#define START_OF_ERRORS_CODES 100

/// перечисление содержащее численные константы для определения ошибок
typedef enum { INCORRECT_BRACKETS, ILLEGAL_CHAR, NO_ARGS,
    FOLLOWING_CONSTANTS, WRONG_LOG_POW, CANNOT_ASSIGN, NOT_ASSIGNED, WRONG_VAL, TOO_MUCH_ARGS } Errors;
/// массив сообщений об ошибках
extern char * errorMesseges[];
/// фунция выводящая сообщене c cоответствующим кодом на экран
void errorLog(int code);


#endif //CPROJECTSCALCULATOR_ERROR_HANDLE_H
