#ifndef CPROJECTSCALCULATOR_FUNC_WRAPS_H
#define CPROJECTSCALCULATOR_FUNC_WRAPS_H

#include <complex.h>

/// массив указателей на функции используется для выполнения указаных действий при вычислении
extern complex double (*funcArray[]) (complex double, complex double);

// функции реализующие возможные действия в калькуляторе
/// сложение
complex double fadd(complex double a, complex double b);
/// выччитание
complex double fsub(complex double a, complex double b);
/// умножение
complex double fmul(complex double a, complex double b);
/// деление
complex double fdev(complex double a, complex double b);
/// возведение в степень
complex double fpow(complex double a, complex double b);
/// синус числа
complex double fsin(complex double a, complex double b);
/// косинус числа
complex double fcos(complex double a, complex double b);
/// тангенс числа
complex double ftan(complex double a, complex double b);
/// логарифм по произвольному основанию
complex double flog(complex double a, complex double b);
/// натуральный логарифм числа
complex double flgn(complex double a, complex double b);
/// квадратный корень из числа
complex double fsrt(complex double a, complex double b);
/// модуль числа
complex double abso(complex double a, complex double b);
/// экспанета числа
complex double expa(complex double a, complex double b);
/// вещественная часть комплексного числа
complex double rily(complex double a, complex double b);
/// мнимая часть комплексного числа
complex double imar(complex double a, complex double b);
/// аргумент комплексного числа
complex double phas(complex double a, complex double b);
/// математическая константа е
complex double econ(complex double a, complex double b);
/// математическая константа PI
complex double picn(complex double a, complex double b);
/// функции не требующие вычисления возвращает 0
complex double nofn(complex double a, complex double b);
#endif //CPROJECTSCALCULATOR_FUNC_WRAPS_H
