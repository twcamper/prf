#ifndef RANDOM_FILE_H
#define RANDOM_FILE_H

#include "prf-config.h"
#include "prf-stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/stat.h>

char *get_random_file(PrfConfig *);

#endif
