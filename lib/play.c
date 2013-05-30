#include "play.h"
#define FLAC "flac"

int play(char *file, char *player)
{
  errno = 0;
  pid_t pid = fork();
  if (pid == -1) {
    perror("Can't fork process to start player");
    return -1;
  }
  if (pid == 0)  {
    errno = 0;
    if (execl(player, player, file, NULL ) == -1)  {
      fprintf(stderr, "Starting %s with %s: %s\n", player, file, strerror(errno));
      return -1;
    }
  }
  return 0;
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
FLAC__uint64 get_duration(char *file, char *ext)
{
  if (strcmp(ext, FLAC) != 0)
    return 0;

  FLAC__StreamMetadata md;
  errno = 0;
  if (!FLAC__metadata_get_streaminfo(file, &md)) {
    fprintf(stderr, "%s:%d getting FLAC metadata streaminfo: %s\n", __FILE__, __LINE__, strerror(errno));
    return -1;
  }

  return (md.data.stream_info.total_samples / md.data.stream_info.sample_rate);
}
