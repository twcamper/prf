#include "played.h"

/* private function prototype declarations */
static int expand_home_dir(char *);
static FILE *get_output_stream(char *);
static int make_dir_p(char *);

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

  FILE *ostream;
  if ((ostream = get_output_stream(log_file)) == NULL)
    return -1;

  size_t output_len = strlen(file_name);
  bool output_error = false;

  if ((size_t)fprintf(ostream, "%s\n", file_name) < output_len) {
    fprintf(stderr, "%s:%d Error writing to %s\n", __FILE__, __LINE__,  log_file);
    output_error = true;
  }
  if (fclose(ostream) == EOF) {
    fprintf(stderr, "%s:%d Error closeing %s\n", __FILE__, __LINE__,  log_file);
    output_error = true;
  }
  if (output_error) {
    return -1;
  }
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
    memmove(filename + home_len, tilde + 1, strlen(tilde + 1));
    /* copy in home path */
    strncpy(filename, home, home_len);
  }

  return 0;
}
static FILE *get_output_stream(char *filename)
{
  /* convert temporarily to the dir path */
  char *final_separator = strrchr(filename, '/');
  *final_separator = '\0';

  struct stat fs;
  errno = 0;
  if (stat(filename, &fs) != 0) {
    if (errno == ENOENT) {  /* we must create the dir(s) */
      if (make_dir_p(filename) != 0)
        return NULL;
    } else {
      perror(filename);
      return NULL;
    }
  }
  /* revert back to full file path */
  *final_separator = '/';

  /* open for appending */
  FILE *fp;
  if (!(fp = fopen(filename, "a"))) {
    fprintf(stderr, "%s:%d Error opening %s\n", __FILE__, __LINE__,  filename);
    return NULL;
  }

  return fp;
}
static int make_dir_p(char *dir_path)
{
  pid_t pid;

  errno = 0;
  if ((pid = fork()) == -1) {
    perror("Can't fork to shell out to mkdir");
    return -1;
  }

  if (pid == 0) {
    errno = 0;
    if (execlp("mkdir", "mkdir", "-p", dir_path, NULL) != 0) {
      perror("mkdir -p");
      return -1;
    }
  }
  return 0;
}
