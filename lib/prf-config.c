#include "prf-config.h"

PrfConfig read_configuration(int argc, char *argv[])
{
  int rc;
  char *dir_glob =  "~/Music/{baroque,rock,rock,rock,jazz,jazz,jazz,soul}";
  PrfConfig prf_config;
  glob_t result;

  if (argc > 0)
    argv[1] = argv[1]; /* hoping to shut up clang for now */

  errno = 0;
  rc = glob(dir_glob, GLOB_BRACE|GLOB_TILDE|GLOB_ERR, NULL, &result);
  if (rc != 0 || (&result && result.gl_pathc == 0)) {
    /* not getting GLOB_NOMATCH, so resort to testing count of paths found */
    fprintf(stderr, "%s:%d glob: %s '%s'\n", __FILE__, __LINE__, strerror(errno), dir_glob);
    exit(EXIT_FAILURE);
  }

  prf_config.entries = glob_to_filtered_entry_list(&result);
  prf_config.extension_count = 1;
  prf_config.ext[0] = "flac";

  return prf_config;
}
