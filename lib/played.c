#include "played.h"

/* private function prototype declarations */
static FILE *get_output_stream(char *);
static int make_dir_p(char *);
static int remove_oldest_lines(char *log_file, uintmax_t limit);

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
int log_as_played(char *data, PrfConfig *config)
{
  /* Quietly exit if there's no log file in the config: */
  /* User does not want to log */
  if (!config->log_file)
    return 0;

  FILE *ostream;
  if ((ostream = get_output_stream(config->log_file)) == NULL)
    return -1;

  /*
   * Append our data (a file name)
   */
  size_t output_len = strlen(data);
  bool output_error = false;

  if ((size_t)fprintf(ostream, "%s\n", data) < output_len) {
    fprintf(stderr, "%s:%d Error writing to %s\n", __FILE__, __LINE__,  config->log_file);
    output_error = true;
  }

  if (fclose(ostream) == EOF) {
    fprintf(stderr, "%s:%d Error closeing %s\n", __FILE__, __LINE__,  config->log_file);
    output_error = true;
  }
  if (output_error) {
    return -1;
  }

  /* remove lines from the top of the file, if necessary */
  if (remove_oldest_lines(config->log_file, config->log_file_limit) != 0)
    return -1;

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
static int remove_oldest_lines(char *log_file, uintmax_t limit)
{
  struct stat fs;
  char *buffer;
  FILE *io;
  int rc = 0;

  if (!limit)
    return rc;

  /* stat file to get size */
  errno = 0;
  if (stat(log_file, &fs) != 0) {
    fprintf(stderr, "%s:%d Error stating %s\n%s\n", __FILE__, __LINE__,  log_file, strerror(errno));
    return -1;
  }

  /* allocate a buffer for the contents */
  errno = 0;
  if (!(buffer = malloc(fs.st_size + 1))) {
    perror("allocating memory to remove oldest lines");
    return -1;
  }

  /* open the stream for Read/Write */
  errno = 0;
  if (!(io = fopen(log_file, "a+"))) {
    fprintf(stderr, "%s:%d Error opening %s\n", __FILE__, __LINE__,  log_file);
    free(buffer);
    return -1;
  }

  /* seek the beginning so we can read now */
  errno = 0;
  if (fseek(io, 0L, SEEK_SET) != 0) {
    fprintf(stderr, "%s:%d Error seeking file start %s\n%s\n", __FILE__, __LINE__,  log_file, strerror(errno));
    rc = -1;
    goto close;
  }
  /* read the whole file */
  errno = 0;
  size_t input_size = (size_t)fs.st_size;
  if (fread(buffer, sizeof(char), input_size, io) < input_size) {
    fprintf(stderr, "%s:%d Error reading file %s\n%s\n", __FILE__, __LINE__,  log_file, strerror(errno));
    rc = -1;
    goto close;
  }
  /* Terminate the string, because fread block IO
   * doesn't know or care about "strings"
   */
  buffer[fs.st_size] = '\0';

  /* count the newlines */
  int newlines = 0;
  char *p = buffer;
  while ((p = strchr(p, '\n'))) {
    p++;
    newlines++;
  }

  /*
   * rewrite the file
   * if there are extra lines to be
   * removed from the beginning
   */
  int extra_lines =  newlines - limit;
  if (extra_lines > 0) {
    /* find start of first line that we want to keep */
    for (newlines = 0, p = buffer; newlines < extra_lines; newlines++, p++)
      p = strchr(p, '\n');

    /* seek the beginning so we can write now */
    errno = 0;
    if (fseek(io, 0L, SEEK_SET) != 0) {
      fprintf(stderr, "%s:%d Error seeking file start %s\n%s\n", __FILE__, __LINE__,  log_file, strerror(errno));
      rc = -1;
      goto close;
    }

    /* empty the file */
    errno = 0;
    if (ftruncate(fileno(io), 0) != 0) {
      fprintf(stderr, "%s:%d Error truncating file %s\n%s\n", __FILE__, __LINE__,  log_file, strerror(errno));
      rc = -1;
      goto close;
    }

    /* write the newer lines */
    errno = 0;
    size_t output_size = fs.st_size - (p - buffer);
    if ((fwrite(p, sizeof(char), output_size, io)) < output_size) {
      fprintf(stderr, "%s:%d Error writing %s\n%s\n", __FILE__, __LINE__,  log_file, strerror(errno));
      rc = -1;
    }
  }

close:

  if (fclose(io) == EOF) {
    fprintf(stderr, "%s:%d Error closeing %s\n", __FILE__, __LINE__, log_file);
    rc = -1;
  }

  free(buffer);
  return rc;
}
