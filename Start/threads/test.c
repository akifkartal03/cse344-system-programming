#include <stdio.h>
#include <string.h>
int a[];
int main ()
{
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