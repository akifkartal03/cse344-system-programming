#ifndef LAGRANGE_FORM_H
#define LAGRANGE_FORM_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 7

double calculateInterpolation(double x[], double y[], int xCount,int count);
double estimationError(double fxCount, double pxCount);
double absolute(double result);
void getCoefficents(double x[], double y[], double coefs[]);
void getCofactor(double mat[N][N], double temp[N][N], int p, int q, int n);
double determinantOfMatrix(double mat[N][N], int n);
void helper(double temp[7][7], double c[15][N], int k, int s);
void printCoofs(double coofs[],int child);


#endif