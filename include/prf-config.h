#ifndef PRF_CONFIG_H
#define PRF_CONFIG_H

#include <stdlib.h>

#define MAX_PATHS 1024      /* researching system defined limit for bash */
#define MAX_EXTENSIONS 256  /* researching system defined limit for bash */
typedef struct prf_config {
  size_t extension_count;
  char *ext[MAX_EXTENSIONS];
  size_t path_count;
  char *path[MAX_PATHS];
} PrfConfig;

PrfConfig read_configuration(int argc, char *argv[]);
#endif
