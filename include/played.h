#ifndef PLAYED_H
#define PLAYED_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "prf-config.h"

int log_as_played(char *data, PrfConfig *);
bool has_been_played(char *, char *);
#endif
