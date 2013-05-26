#include "played.h"

/* private function prototype declarations */
static int expand_home_dir(char *);

int log_as_played(char *file_name, char *log_file)
{
  /* 
   * if first dir node in path is ~
   *    getenv HOME
   *    prepend to path
   * if log file does not exist
   *    create new log file
   *       shell out to mkdir -p
   *       handle inheriting file permissions if necessary
   * fopen(path, "a")
   *
   * write line (file_name arg) to end
   *
   * close file
   */

  /* Quietly exit if there's no log file in the config: */
  /* User does not want to log */
  if (strlen(log_file) == 0)
    return 0;

  if (expand_home_dir(log_file) != 0)
    return -1;

  struct stat fs;
  if (stat(log_file, &fs) != 0)
    perror(log_file);

  return 0;
}
static int expand_home_dir(char *filename)
{
  char *tilde = strchr(filename, '~');
  if (tilde) {
    char *home = getenv("HOME");
    size_t home_len = strlen(home);

    /* make sure we have room */
    if (home_len + strlen(filename) > FILENAME_MAX) {
      errno = ENAMETOOLONG;
      fprintf(stderr, "%s:%d expand_home_dir() %s: '%s' + '%s'\n", __FILE__, __LINE__, strerror(errno), home, tilde + 1);
      return -1;
    }
    /* shift string down the array to accommadate home path */
    memmove(filename + home_len, tilde + 1, home_len);
    /* copy in home path */
    strncpy(filename, home, home_len);
  }

  return 0;
}
