#ifndef LAGRANGE_FORM_H
#define LAGRANGE_FORM_H
#include <stdio.h>
#include <stdlib.h>

double calculateInterpolation(double x[], double y[], int xCount,int count);
double estimationError(double fxCount, double pxCount);
double absolute(double result);




#endif