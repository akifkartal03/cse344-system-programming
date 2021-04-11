#include "lagrange.h"

double calculateInterpolation(double x[], double y[], int xCount, int count)
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
double estimationError(double fxCount, double pxCount)
{
    return absolute(fxCount - pxCount);
    
}
double absolute(double result)
{
    return (result < 0) ? result * -1 : result;
}
void getCoefficents(double x[], double y[], double coefs[])
{
    double m[N][N];
    double c[15][N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            double xi = x[j];

            double res = pow(xi, i) * pow(-1, (i * 7));
            m[i][j] = res;
            c[i][j] = res;
        }
    }
    for (int j = 0; j < N; j++)
    {
        c[7][j] = y[j];
    }
    for (int i = 8; i < 15; i++)
    {
        for (int j = 0; j < N; j++)
        {

            c[i][j] = m[i - 8][j];
        }
    }
    double cof[N + 1];
    for (int i = 0; i < N + 1; i++)
    {
        double temp[7][7];
        helper(temp, c, i, i + 7);
        cof[i] = determinantOfMatrix(temp, N);
    }
    double realcof[N];
    double var = cof[0];
    for (int i = 0; i < N + 1; i++)
    {
        double a = var * pow(-1, 8);
        cof[i] = cof[i] / a;
        if (i != 0)
        {
            coefs[i - 1] = cof[i];
        }
    }
}
void getCofactor(double mat[N][N], double temp[N][N], int p, int q, int n)
{
    int i = 0, j = 0;

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {

            if (row != p && col != q)
            {
                temp[i][j++] = mat[row][col];

                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}
double determinantOfMatrix(double mat[N][N], int n){
    double D = 0.0;

    if (n == 1)
        return mat[0][0];

    double temp[N][N];

    int sign = 1;

    for (int f = 0; f < n; f++)
    {

        getCofactor(mat, temp, 0, f, n);
        D += sign * mat[0][f] * determinantOfMatrix(temp, n - 1);

        sign = -sign;
    }

    return D;
}
void helper(double temp[7][7], double c[15][N], int k, int s){
    int m = 0;
    for (int i = 0 + k; i < s; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            temp[m][j] = c[i][j];
        }
        m++;
    }
}
void printCoofs(double coofs[],int child){
    printf("Polynomial %d: ",child);
    for (int i = 0; i < 7; i++)
    {
        if (i!=6)
        {
            printf("%.1f,",coofs[i]);
        }
        else{
            printf("%.1f",coofs[i]);
        }    
    }
    printf("\n");
    
}