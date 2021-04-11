#include "lagrange.h"

double calculateInterpolation(double x[], double y[], int xCount,int count)
{
    double yResult = 0;
    for (int i = 0; i < count; i++)
    {
        double nextY = y[i];
        for (int j = 0; j < count; j++)
        {
            if (j != i)
                nextY = nextY * (xCount - x[j]) / (x[i] - x[j]);
        }
        yResult = yResult + nextY;
    }
    return yResult;
}
double estimationError(double fxCount, double pxCount){
    return absolute(fxCount-pxCount);
}
double absolute(double result){
    return (result < 0) ? result*-1 : result;
}
double[][] matrix1(double x[], double y[]){
    double m[N][N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            double xi = x[j];
            double res = pow(pow(xi,i)*(-1),i*7);
            m[i][j] = res;
        }
        
    }
    
    
}
double[][] matrix2(double x[]);
void getCofactor(int mat[][], int temp[][], int p,int q, int n);
int determinantOfMatrix(int mat[N][N], int n);
double[] getCoefficents(int lineNumber);