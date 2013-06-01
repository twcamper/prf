#ifndef PRF_CONFIG_H
#define PRF_CONFIG_H

#if defined(__linux__) || defined(__linux) || defined(__gnu_linux__)
  #ifndef _BSD_SOURCE
    #define _BSD_SOURCE 1           /* strdup in string.h */
  #endif
  #ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 2       /* getopt, etc. in unistd.h */
  #endif
#endif

#include <ctype.h>
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "filtered-entry-list.h"
#include "prf-validation.h"

#define MAX_PATHS 1024      /* researching system defined limit for bash */
#define MAX_EXTENSIONS 256
#define EMPTY_CONFIG_FILE -13
#define DEFAULT_CONFIG_FILE_PATH "~/.prfrc"

typedef struct prf_config {
  size_t extension_count;
  char *ext[MAX_EXTENSIONS];
  size_t association_count;
  char *associations[MAX_EXTENSIONS][2];
  char *log_file;
  uintmax_t log_file_limit;
  FilteredEntryList  entries;
} PrfConfig;

PrfConfig read_configuration(int *argc, char **argv[]);
void      destroy_configuration(PrfConfig *);
#endif
