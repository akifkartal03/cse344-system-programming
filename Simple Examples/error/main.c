#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define N 7
void getCofactor(double mat[N][N], double temp[N][N], int p,
                 int q, int n)
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

double determinantOfMatrix(double mat[N][N], int n)
{
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
void getdo(double temp[7][7], double c[15][N], int k, int s)
{
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
int main()
{
    double x[7] = {6.0, 1.0, 5.0, 9.0, 8.0, 7.0, 4.0};
    double y[7] = {8.0, 12.0, 5.0, 7.0, 2.0, 4.0, 6.0};
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
        getdo(temp, c, i, i + 7);
        cof[i] = determinantOfMatrix(temp, N);
        //printf("%.1f\n",cof[i]);
    }
    double realcof[N];
    double var = cof[0];
    for (int i = 0; i < N + 1; i++)
    {
        double a = var * pow(-1, 8);
        cof[i] = cof[i] / a;
        //printf("%.1f\n",cof[i]);
        if (i != 0)
        {
            realcof[i - 1] = cof[i];
            printf("%.1f\n", realcof[i - 1]);
        }
    }
    return 0;
}
