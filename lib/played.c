#include "played.h"

/* private function prototype declarations */
static FILE *get_output_stream(char *);
static int make_dir_p(char *);

bool has_been_played(char *data, char *log_file)
{
  /* line from the log file: a file name */
  char line[FILENAME_MAX + 1];
  bool played = false;
  bool read_error = false;
  FILE *istream;
  size_t data_len;

  /* Quietly exit if there's no log file in the config: */
  /* User does not want to log */
  if (!log_file)
    return played;

  errno = 0;
  if (!(istream = fopen(log_file, "r"))) {
    /* Quietly exit if this is the first run, and there's no file to read yet */
    if (errno == ENOENT) {
      return played;
    } else {
      perror(log_file);
      return -1;
    }
  }

  /* if the file is listed in the log, it has been played */
  data_len = strlen(data);
  errno = 0;
  while(fgets(line, FILENAME_MAX, istream)) {
    /* line read with '\n', so limit the comparison */
    if (strncmp(line, data, data_len) == 0) {
      played = true;
      break;
    }
  }
  if (ferror(istream)) {
    perror(log_file);
    read_error = true;
  }
  errno = 0;
  if (fclose(istream) == EOF || read_error) {
    perror(log_file);
    return -1;
  }
  return played;
}
int log_as_played(char *file_name, char *log_file)
{
  /* Quietly exit if there's no log file in the config: */
  /* User does not want to log */
  if (!log_file)
    return 0;

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
  int pidstatus;

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
  errno = 0;
  if (waitpid(pid, &pidstatus, 0) == -1) {
    perror("waiting for mkdir -p");
    return -1;
  }
  return 0;
}
