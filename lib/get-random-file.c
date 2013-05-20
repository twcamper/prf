#include "get-random-file.h"
#define FILE_SEPARATOR "/"

static bool can_play(char *ext[], size_t n, char *filename)
{
  char *p;
  for (size_t i = 0; i < n; i++)
    if ((p = strrchr(filename, '.')))
      if (strcmp(++p, ext[i]) == 0)
        return true;

  return false;
}
static bool already_tried(size_t index, size_t tried_list[], size_t n)
{
  for (size_t i = 0; i < n; i++)
    if (tried_list[i] == index)
      return true;

  return false;
}
static char *find_file(glob_t entries, char **ext, size_t extension_count)
{
  size_t tried[BUFSIZ], tried_count = 0;
  size_t entry;
  char dir_glob[PATH_MAX + 2];
  struct stat fs;

  while (tried_count < entries.gl_pathc) {
    /* pick random entry */
    entry = rand() % entries.gl_pathc;

    /* keep trying if this one already failed */
    if (already_tried(entry, tried, tried_count))
      continue;

    /* store in list of those tried */
    tried[tried_count++] = entry;

    /* stat the file we just found */
    if (stat(entries.gl_pathv[entry], &fs) != 0) {
      perror(entries.gl_pathv[entry]);
      exit(EXIT_FAILURE);
    }

    /* get new set of entries if it's another directory */
    if (S_ISDIR(fs.st_mode)) {
      strcpy(dir_glob, entries.gl_pathv[entry]);
      strncat(dir_glob, FILE_SEPARATOR, 1);
      strncat(dir_glob, "*", 1);
      errno = 0;
      if ((glob(dir_glob, GLOB_BRACE|GLOB_ERR, NULL, &entries) != 0
           || (&entries && entries.gl_pathc == 0))) {
        /* not getting GLOB_NOMATCH, so resort to testing count of paths found */
        fprintf(stderr, "%s:%d glob: %s '%s'\n", __FILE__, __LINE__, strerror(errno), dir_glob);
        exit(EXIT_FAILURE);
      }

      /* RECURSION */
      return find_file(entries, ext, extension_count);
    }
    /* we're done if it's a regular file with a good extension */
    if (S_ISREG(fs.st_mode)) {
      if (can_play(ext, extension_count, entries.gl_pathv[entry])) {
        return entries.gl_pathv[entry];
      }
    }
  }
  return NULL;
}

char *get_random_file(PrfConfig config)
{
  srand((unsigned int) time(NULL));
  return find_file(config.entries, config.ext, config.extension_count);
}
