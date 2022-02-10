#include <stdio.h>
#include <assert.h>
#include "error_handle.h"


//массив строк вывода ошибок
char * errorMesseges[] = { "Error: Entered expression has incorrect bracket sequence!\n",
                           "Error: character is not allowed!\n",
                           "Error: No arguments passed!\n",
                           "Error: is not a single expression, two numbers or variables without operator!\n",
                           "Error: wrong log() or pow() format!\n",
                           "Error! left part of assigning has to be variable!\n",
                           "Error! using variable before assignment!\n",
                           "Error! can not calculate expression! Zero division, log of zero or tg of PI / 2!\n",
                           "Error! too much arguments for function!\n"};
// функция принимает код ошибки,
// коды определены в заголовочном файле, выводит нужное сообщение и выходит с нужным кодом
void errorLog(int code){
    // INCORRECT_BRACKETS - ошибка с наименьшим кодом остальные следуют попорядку
    printf("%s", errorMesseges[code]);
    exit(code + START_OF_ERRORS_CODES);
}
