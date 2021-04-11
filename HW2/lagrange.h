#ifndef LAGRANGE_FORM_H
#define LAGRANGE_FORM_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 7
double calculateInterpolation(double x[], double y[], int xCount,int count);
double estimationError(double fxCount, double pxCount);
double absolute(double result);
double[][] matrix1(double x[], double y[]);
double[][] matrix2(double x[]);
void getCofactor(int mat[][], int temp[][], int p,int q, int n);
int determinantOfMatrix(int mat[N][N], int n);
double[] getCoefficents(int lineNumber);



#endif