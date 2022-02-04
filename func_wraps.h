#ifndef CPROJECTSCALCULATOR_FUNC_WRAPS_H
#define CPROJECTSCALCULATOR_FUNC_WRAPS_H

#include <complex.h>

// array of function pointers to perform needed functions
extern complex double (*funcArray[]) (complex double, complex double);

complex double fadd(complex double a, complex double b);
complex double fsub(complex double a, complex double b);
complex double fmul(complex double a, complex double b);
complex double fdev(complex double a, complex double b);
complex double fpow(complex double a, complex double b);
complex double fsin(complex double a, complex double b);
complex double fcos(complex double a, complex double b);
complex double ftan(complex double a, complex double b);
complex double flog(complex double a, complex double b);
complex double flgn(complex double a, complex double b);
complex double fsrt(complex double a, complex double b);
complex double abso(complex double a, complex double b);
complex double expa(complex double a, complex double b);
complex double rily(complex double a, complex double b);
complex double imar(complex double a, complex double b);
complex double phas(complex double a, complex double b);
complex double econ(complex double a, complex double b);
complex double picn(complex double a, complex double b);

#endif //CPROJECTSCALCULATOR_FUNC_WRAPS_H
