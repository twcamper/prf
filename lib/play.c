#include "play.h"

int play(char *file, PrfConfig *config)
{
  char *player;
  char *dot = strrchr(file, '.');
  for (size_t i = 0; i < config->association_count; i++)
    if (strcmp(dot + 1,  config->associations[i][0]) == 0) {
      player = config->associations[i][1];
      break;
    }

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
