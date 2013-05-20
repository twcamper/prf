#ifndef PRF_CONFIG_H
#define PRF_CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <glob.h>
#include <errno.h>
#include <string.h>

#define MAX_PATHS 1024      /* researching system defined limit for bash */
#define MAX_EXTENSIONS 256  /* researching system defined limit for bash */
typedef struct prf_config {
  size_t extension_count;
  char *ext[MAX_EXTENSIONS];
  glob_t entries;
} PrfConfig;

PrfConfig read_configuration(int argc, char *argv[]);
#endif
