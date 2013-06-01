#ifndef PLAYED_H
#define PLAYED_H

#if defined(__linux__) || defined(__linux) || defined(__gnu_linux__)
  #ifndef _BSD_SOURCE
    #define _BSD_SOURCE 1           /* ftruncate() */
  #endif
  #ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 2       /*  fileno() */
  #endif
  #include <sys/types.h>            /* pid_t */
  #include <sys/wait.h>             /* waitpid() */
#endif

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
