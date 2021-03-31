#ifndef MY_PRINT_H
#define MY_PRINT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>


void my_printf(const char *str);
void my_stderr(const char *str);

#endif
