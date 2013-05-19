#ifndef PRF_CONFIG_H
#define PRF_CONFIG_H

#define MAX_PATHS 1024      /* researching system defined limit for bash */
#define MAX_EXTENSIONS 256  /* researching system defined limit for bash */
typedef struct prf_config {
  char *ext[MAX_EXTENSIONS];
  char *path[MAX_PATHS];
} PrfConfig;

PrfConfig read_configuration(int argc, char *argv[]);
#endif
