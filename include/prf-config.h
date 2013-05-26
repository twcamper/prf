#ifndef PRF_CONFIG_H
#define PRF_CONFIG_H

#include <ctype.h>
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "filtered-entry-list.h"

#define MAX_PATHS 1024      /* researching system defined limit for bash */
#define MAX_EXTENSIONS 256
#define EMPTY_CONFIG_FILE -13
#define CONFIG_FILE_PATH "~/.prfrc"

typedef struct prf_config {
  size_t extension_count;
  char *ext[MAX_EXTENSIONS];
  size_t association_count;
  char *associations[MAX_EXTENSIONS][2];
  char log_file[FILENAME_MAX + 1];
  FilteredEntryList  entries;
} PrfConfig;

PrfConfig read_configuration(int *argc, char **argv[]);
void      destroy_configuration(PrfConfig *);
#endif
