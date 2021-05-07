#include <stdio.h>
#include <string.h>
int n;
int a[n];
int main ()
{
  n = 5;
  
  char str[] ="This is a sample string.\n";
  char * pch;
  pch = strtok (str," \n");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    pch = strtok (NULL, " \n");
  }
  return 0;
}