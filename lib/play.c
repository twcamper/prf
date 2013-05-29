#include "play.h"

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
