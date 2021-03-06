#include "prf-config.h"
#include "random-file.h"
#include "play.h"
#include "played.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int rc = 0;
  PrfConfig config = read_configuration(&argc, &argv);
  char *file = get_random_file(&config);

  if (register_signal_handler(SIGINT, kill_player) == -1) {
    perror("setting signal handler 'kill_player'");
    exit(EXIT_FAILURE);
  }

  if (file) {
    char *ext    = get_extension(file);
    char *player = get_player(ext, &config);
    unsigned long long duration = get_duration(file, ext);

    if (config.print)
      printf("%s: %lld\n", file, duration);

    if (player && (kill_all(player) == 0) && duration != 1) {
      if (duration && (register_signal_handler(SIGALRM, catch_alarm) == 0))
        alarm(duration + 3);  /* pad the end a little */

      if (play(file, player) == 0)
        log_as_played(file, &config);
    }
  } else {
    printf("No files found.\n");
    rc = EXIT_FAILURE;
  }

  free(file);
  destroy_configuration(&config);
  return rc;
}
