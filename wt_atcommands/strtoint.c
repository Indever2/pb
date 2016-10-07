#include <string.h>

/* transfering char * to int (this func has no input check!) */
int strtoint(const char * in) { 
  int i = 0, j = 0;
  /* multipler - indeed to right arrangment of digits inside the final num */
  int len = 0, sum = 0, multipler = 1; 
  len = strlen(in);

  for (i = 0; i < len; i++) {
    for (j = 0; j < i; j++)
      /* After every iteration multipler taking 10-fold increase */
      multipler *= 10; 

    /* Adding to the sum the num obtained by using multiplication extracted symbol to multipler. */
    sum += (in[len - i - 1] - '0') * multipler; 
    /* Zeroing the multiplier */
    multipler = 1; 
  }
  return sum;
}
