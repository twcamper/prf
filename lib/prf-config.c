#include "prf-config.h"

PrfConfig read_configuration(int argc, char *argv[])
{
  int rc;
  char *dir_glob =  "~/Music/{baroque,rock,jazz,soul}";
  PrfConfig prf_config;

  if (argc > 0)
    argv[1] = argv[1]; /* hoping to shut up clang for now */

  errno = 0;
  rc = glob(dir_glob, GLOB_BRACE|GLOB_TILDE|GLOB_ERR, NULL, &prf_config.entries);
  if (rc != 0 || (&prf_config.entries && prf_config.entries.gl_pathc == 0)) {
    /* not getting GLOB_NOMATCH, so resort to testing count of paths found */
    fprintf(stderr, "%s:%d glob: %s '%s'\n", __FILE__, __LINE__, strerror(errno), dir_glob);
    exit(EXIT_FAILURE);
  }

  prf_config.extension_count = 1;
  prf_config.ext[0] = "flac";

  return prf_config;
}
