#include "play.h"

/* private macro definitions */
#define FLAC "flac"
#define MP3  "mp3"

/* private function prototype declarations */
static FLAC__uint64 flac_duration(char *file);
static long int mp3_duration(char *file);

/* GLOBALS */
pid_t PRF__global_player_pid;  /* global for access by signal handler */


/* public function definitions */
int play(char *file, char *player)
{
  int pid_status;
  errno = 0;
  PRF__global_player_pid = fork();
  if (PRF__global_player_pid == -1) {
    perror("Can't fork process to start player");
    return -1;
  }
  if (PRF__global_player_pid == 0)  {
    errno = 0;
    if (execl(player, player, file, NULL ) == -1)  {
      fprintf(stderr, "Starting %s with %s: %s\n", player, file, strerror(errno));
      return -1;
    }
  }
  errno = 0;
  if (waitpid(PRF__global_player_pid, &pid_status, 0) == -1) {
    /* We expect to be killed because of a SIGALRM set in main():
     *
     * see kill_player() and register_signal_handler()
     *
     * */
    if (errno != EINTR) {
      /* any other error would be bad */
      perror("wait for child player process");
      return -1;
    }
  }

  return WEXITSTATUS(pid_status);
}
char *get_player(char *ext, PrfConfig *config)
{
  for (size_t i = 0; i < config->association_count; i++)
    if (strcmp(ext,  config->associations[i][0]) == 0) {
      return config->associations[i][1];
    }
  fprintf(stderr, "No player for '%s' files\n", ext);
  return NULL;
}
char *get_extension(char *file)
{
  static char *dot;
  dot = strrchr(file, '.');
  return ++dot;
}
int kill_all(char *player)
{
  char *player_name = strrchr(player, '/');
  player_name++;

  /* we do it like this because killall raises an error on no process found */
  char kill_script[256] = {'\0'};
  sprintf(kill_script, "for p in `pgrep %s`; do kill -KILL $p; done", player_name);

  errno = 0;
  int pid_status;
  pid_t pid = fork();
  if (pid == -1) {
    perror("Can't fork process to kill player");
    return -1;
  }
  if (pid == 0)  {
    errno = 0;
    if (execlp("bash", "bash", "-c", kill_script, NULL) == -1)  {
      fprintf(stderr, "Killing %s: %s\n", player_name, strerror(errno));
      return -1;
    }
  }

  errno = 0;
  if (waitpid(pid, &pid_status, 0) == -1) {
    perror("wait for child kill script process");
    return -1;
  }

  return WEXITSTATUS(pid_status);
}
time_t get_duration(char *file, char *ext)
{
  if (strcmp(ext, FLAC) == 0)
    return (time_t)flac_duration(file);

  if (strcmp(ext, MP3) == 0)
    return (time_t)mp3_duration(file);

  return 0;
}

void catch_alarm(int signal)
{
  errno = 0;
  if (raise(SIGINT) == -1) {
    printf("%d:\t", signal);
    perror("catch_alarm");
  }
}
void kill_player(int signal)
{
  errno = 0;
  if (kill(PRF__global_player_pid, SIGKILL) == -1) {
    printf("%d:\t", signal);
    perror("kill_player");
  }
}
int register_signal_handler(int signal, void (*handler)(int))
{
  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  errno = 0;
  if (sigaction(signal, &action, NULL) == -1) {
    fprintf(stderr, "%s:%d setting signal handler: %s\n", __FILE__, __LINE__, strerror(errno));
    return -1;
  }

  return 0;
}

/* private function definitions */
static FLAC__uint64 flac_duration(char *file)
{
  FLAC__StreamMetadata md;
  errno = 0;
  if (!FLAC__metadata_get_streaminfo(file, &md)) {
    fprintf(stderr, "%s:%d getting FLAC metadata streaminfo: %s\n", __FILE__, __LINE__, strerror(errno));
    /* positive error code because we return an unsigned type */
    return 1;
  }

  return (md.data.stream_info.total_samples / md.data.stream_info.sample_rate);
}
static long int mp3_duration(char *file)
{
  FILE *pipe;
  char cmd[FILENAME_MAX + 16];
  static long int seconds;;

  /* quoting file name in case of spaces for sh and bash */
  sprintf(cmd,"mp3info -p \"%%S\" \"%s\"", file);

  errno = 0;
  if ((pipe = popen(cmd, "r")) == NULL) {
    perror("mp3_duration: create pipe");
    return 1;
  }
  errno = 0;
  if (fscanf(pipe, "%ld", &seconds) != 1) {
    perror("mp3_duration: read from pipe");
    seconds = 1;
  }
  errno = 0;
  if (pclose(pipe) == EOF) {
    perror("mp3_duration: close pipe");
    seconds = 1;
  }

  return seconds;
}
