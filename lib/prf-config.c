#include "prf-config.h"

/*
 * private macro definitions
 */
#define LINE_MAX 1024
#define EXT "ext"
#define PATH "path"
#define PATTERN "pattern"
#define ASSOCIATION "association"
#define ITEM_DELIMITERS ":,;"

/*
 * private function declarations
 */
static size_t add_associations(PrfConfig *, char *);
static int append_delimited_list(glob_t *, char *);
static void exit_error(char *, int , char *);
static size_t parse_delimited_list(char *, char **);
static int read_config_file(PrfConfig *);
static void set_config_filename(char *);
static char *value_for_key(char *, char *);

/*
 * private function definitions
 */
static size_t add_associations(PrfConfig *config, char *s)
{
  size_t len, new_assocations = 0;
  char *ext, *player;
  if ((len = strlen(s)) == 0)
    return -1;

  /* absolute path to a player */
  if (!(player = strtok(s, ITEM_DELIMITERS)))
    return -1;

  /* could be multiple associations to one player */
  while ((ext = strtok(NULL, ITEM_DELIMITERS))) {
    config->associations[config->association_count][0] = strdup(ext);
    config->associations[config->association_count][1] = strdup(player);
    config->association_count++;
    new_assocations++;
  }
  return new_assocations;
}
static void exit_error(char *f, int l, char *msg)
{
  fprintf(stderr, "%s:%d\t", f, l);
  perror(msg);
  exit(EXIT_FAILURE);
}
static char * value_for_key(char *s, char *key)
{
  size_t len = strlen(key);
  /* advance pointer past end of key-word */
  s += len + 1;

  /* clean up trailing whitespace */
  len = strlen(s);
  while (isspace(s[--len]))
    s[len] = '\0';

  /* first char not in that string: should begin our value */
  return (s += strspn(s, " \t="));
}
static int append_delimited_list(glob_t *g, char *s)
{
  int rc = 0;
  char *p;
  if (strlen(s) == 0)
    return -1;

  p = strtok(s, ITEM_DELIMITERS);
  while (p) {
    errno = 0;
    rc = glob(p, GLOB_APPEND|GLOB_BRACE|GLOB_TILDE|GLOB_ERR, NULL, g);
    if (rc != 0 || g->gl_pathc == 0) {
      exit_error(__FILE__, __LINE__,  s);
    }
    p = strtok(NULL, ITEM_DELIMITERS);
  }
  return rc;
}
static size_t parse_delimited_list(char *s, char **ext)
{
  size_t len, tokens = 0;
  char *p;
  if ((len = strlen(s)) == 0)
    return tokens;

  ext[tokens++] = strdup(strtok(s, ITEM_DELIMITERS));
  while ((p = strtok(NULL, ITEM_DELIMITERS))) {
    ext[tokens++] = strdup(p);
  }
  return tokens;
}
static void set_config_filename(char *buffer)
{
  char *tilde = strchr(CONFIG_FILE_PATH, '~');
  if (tilde) {
    strncpy(buffer, getenv("HOME"), FILENAME_MAX + 1);
    strncat(buffer, tilde + 1, FILENAME_MAX + 1);
  } else
    strncpy(buffer, CONFIG_FILE_PATH, FILENAME_MAX + 1);
}
static int read_config_file(PrfConfig *config)
{
  char line[LINE_MAX + 1];
  char filename[FILENAME_MAX + 1];
  bool read_error = false;
  struct stat fs;
  int rc;
  glob_t result;

  memset(&result, 0, sizeof(result));
  memset(&fs, 0, sizeof(fs));

  set_config_filename(filename);

  if (stat(filename, &fs)) {
    return errno;
  }

  if (fs.st_size == 0) {
    return EMPTY_CONFIG_FILE;
  }

  FILE *fp;
  if ((fp = fopen(filename, "r")) == NULL) {
    exit_error(__FILE__, __LINE__, filename);
  }

  char *l;
  while ((l = fgets(line, sizeof(line), fp)) != NULL) {
    /* ignore leading whitespace */
    while (*l && *l != '\n' && isspace(*l))
      l++;

    /* skip commented or blank line */
    if (*l == '#' || *l == '\n')
      continue;

    if (config->extension_count == 0) {
      if (strncmp(EXT, l, 3) == 0) {
        l = value_for_key(l, EXT);
        if (*l == '\0') {
          read_error = true;
          break;
        }
        config->extension_count = parse_delimited_list(l, config->ext);
        continue;
      }
    }

    if (config->entries == NULL) {
      if (strncmp(PATH, l, 4) == 0) {
        l = value_for_key(l, PATH);
        if (*l == '\0') {
          read_error = true;
          break;
        }
        /* break up paths, feed to vector_to_filtered_entry_list */
        if (append_delimited_list(&result, l) != 0)
          exit_error(__FILE__, __LINE__, "empty 'path' value string");
        continue;
      }

      if (strncmp(PATTERN, l, 7) == 0) {
        l = value_for_key(l, PATTERN);
        if (*l == '\0') {
          read_error = true;
          break;
        }

        errno = 0;
        rc = glob(l, GLOB_APPEND|GLOB_BRACE|GLOB_TILDE|GLOB_ERR, NULL, &result);
        if (rc != 0 || result.gl_pathc == 0) {
          /* not getting GLOB_NOMATCH, so resort to testing count of paths found */
          exit_error(__FILE__, __LINE__,  l);
        }
        continue;
      }
    }

    if (strncmp(ASSOCIATION, l, 6) == 0) {
        l = value_for_key(l, ASSOCIATION);
        if (*l == '\0') {
          read_error = true;
          break;
        }
        if (add_associations(config, l) < 1) {
          fprintf(stderr, "%s:%d bad association line\n", __FILE__, __LINE__);
          read_error = true;
          break;
        }
    }
  }
  if (ferror(fp)) {
    read_error = true;
    perror(filename);
  }
  if (fclose(fp) == EOF || read_error) {
    exit_error(__FILE__, __LINE__, filename);
  }
  if (result.gl_pathc) {
    config->entries = vector_to_filtered_entry_list(result.gl_pathv, result.gl_pathc);
    globfree(&result);
  }

  return 0;
}

/*
 * Public Functions
 */

/* argc and argv should be pointers so main will have know about any changes */
PrfConfig read_configuration(int *argc, char **argv[])
{
  int opt_ch, rc;
  PrfConfig config;
  memset(&config, 0, sizeof(PrfConfig));

  if (*argc > 1) {
    while ((opt_ch = getopt(*argc, *argv, "e:")) != EOF) {
      switch(opt_ch) {
        case 'e':
          config.extension_count = parse_delimited_list(optarg, config.ext);
          break;
        default:
          fprintf(stderr, "Unknown option: '%c'\n", opt_ch);
          exit(EXIT_FAILURE);
      }
    }
    /* point past program name in args to vector_to_filtered_entry_list */
    size_t arg_offset = 1;

    /* argv will point to first arg AFTER program name!! */
    if (optind > 1) {
      *argc -= optind;
      *argv += optind;
      arg_offset = 0;  /* program name is no more!! */
    }
    /* make an entry list if we have non-option args */
    if (*argc > 0)
      config.entries = vector_to_filtered_entry_list(*argv + arg_offset, *argc - arg_offset);
  }

  rc = read_config_file(&config);
  if (config.extension_count == 0 || (!config.entries)) {
    if (rc == EMPTY_CONFIG_FILE)
      fprintf(stderr, "empty config file: %s\n", CONFIG_FILE_PATH);
    else if (rc != 0)
      fprintf(stderr, "%s: %s\n", strerror(rc),  CONFIG_FILE_PATH);

    destroy_configuration(&config);
    exit(EXIT_FAILURE);
  }

  return config;
}
void destroy_configuration(PrfConfig *config)
{
  for (size_t i = 0; i < config->extension_count; i++)
    free(config->ext[i]);
  for (size_t i = 0; i < config->association_count; i++) {
    free(config->associations[i][0]);
    free(config->associations[i][1]);
  }
}
