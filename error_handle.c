#include <stdio.h>
#include <assert.h> //тут лежит exit
#include "error_handle.h"


//массив строк вывода ошибок
char * errorMessages[] = {"Error: Entered expression has incorrect bracket sequence!\n",  //неправильная скобочная последовательность
                           "Error: character is not allowed!\n", //символ запрещен(!,?, любое что не цифра,буква,операторы)
                           "Error: No arguments passed!\n", //пустые скобки или нехватка аргументов(у возведения в степень слева ничего не стоит)
                           "Error: is not a single expression, two numbers or variables without operator!\n", //нет знака между переменными или числами
                           "Error: wrong log() or pow() format!\n", //неправильный формат у логарифма и возведения в ст.
                           //должны быть 2 аргумента разделенные запятой
                           "Error! left part of assigning has to be variable!\n", //слева от равно должна быть переменная
                           "Error! using variable before assignment!\n", //использование переменной перед присваиванием
                           //(ещё значение не присвоено, а она уже используется)
                           "Error! can not calculate expression! Zero division, log of zero or tg of PI / 2!\n",
                           //деление на ноль, логарифм 0,
                           "Error! too much arguments for function!\n"}; //слишком много аргументов для функции
                           // (например в синусе 2 аргумента)
// функция принимает код ошибки,
// коды определены в заголовочном файле, выводит нужное сообщение и выходит с нужным кодом
void errorLog(int code){
    // INCORRECT_BRACKETS - ошибка с наименьшим кодом остальные следуют по порядку
    printf("%s", errorMessages[code]);
    exit(code + START_OF_ERRORS_CODES);
}
