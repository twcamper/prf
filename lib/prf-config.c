#include "prf-config.h"

static size_t parse_delimited_list(char *s, char **ext)
{
  size_t tokens = 0;
  char *p;
  if (strlen(s) == 0)
    return tokens;

  ext[tokens++] = strtok(s, ":,;");
  while ((p = strtok(NULL, ":,;"))) {
    ext[tokens++] = p;
  }
  return tokens;
}
/* argc and argv should be pointers so main will have know about any changes */
PrfConfig read_configuration(int *argc, char **argv[])
{
  int opt_ch, rc;
  char *dir_glob =  NULL;
  /* char *dir_glob =  "~/Music/{baroque,moderne,rock,rock,rock,jazz,jazz,jazz,soul}"; */
  PrfConfig prf_config;
  memset(&prf_config, 0, sizeof(PrfConfig));
  glob_t result;

  if (*argc > 0) {
    while ((opt_ch = getopt(*argc, *argv, "e:")) != EOF) {
      switch(opt_ch) {
        case 'e':
          prf_config.extension_count = parse_delimited_list(optarg, prf_config.ext);
          break;
        default:
          fprintf(stderr, "Unknown option: '%c'\n", opt_ch);
          exit(EXIT_FAILURE);

      }
    }

    if (optind > 1) {
      *argc -= optind;
      *argv += optind;
    }
    prf_config.entries = vector_to_filtered_entry_list(*argv + 1, *argc - 1);
  }

  if (dir_glob) {
    errno = 0;
    rc = glob(dir_glob, GLOB_BRACE|GLOB_TILDE|GLOB_ERR, NULL, &result);
    if (rc != 0 || (&result && result.gl_pathc == 0)) {
      /* not getting GLOB_NOMATCH, so resort to testing count of paths found */
      fprintf(stderr, "%s:%d glob: %s '%s'\n", __FILE__, __LINE__, strerror(errno), dir_glob);
      exit(EXIT_FAILURE);
    }

    prf_config.entries = vector_to_filtered_entry_list(result.gl_pathv, result.gl_pathc);
  }

  if (!prf_config.extension_count) {
    prf_config.extension_count = 3;
    prf_config.ext[0] = "flac";
    prf_config.ext[1] = "mp3";
    prf_config.ext[2] = "wv";
  }

  return prf_config;
}
