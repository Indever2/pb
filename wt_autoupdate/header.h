#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define INCORRECT_EXPRESSION   -1
#define CORRECT_EXPRESSION      1

void debug_print(char *);
void need_update(char **, struct tm *, time_t);
char * slashes_distroyer(char *, int);
int state_validator(char *);
int validator(char *);
int strtoint(char *);
