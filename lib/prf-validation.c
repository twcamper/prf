#include "prf-validation.h"

#define BASE_TEN 10

int to_valid_uint(uintmax_t *n, char *str, int length)
{
  char *endptr;
  int length_converted;
  *n = 0;

  if (str[0] == '-')
    return -1;

  errno = 0;
  *n = strtoull(str, &endptr, BASE_TEN);

  if (errno != 0)
    return -2;

  /* did we stop reading early because of non-numerics? */
  length_converted = endptr - str;
  if (length_converted != length)
    return -3;

  return 0;
}
