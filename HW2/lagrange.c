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