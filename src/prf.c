#include "prf-config.h"
#include "random-file.h"
#include "play.h"
#include "played.h"

int main(int argc, char *argv[])
{
  PrfConfig config = read_configuration(&argc, &argv);
  char *file = get_random_file(&config);

  if (file) {
    printf("%s\n", file);
    if (play(file, &config) == 0)
      log_as_played(file, config.log_file);
  } else
    printf("No files found.\n");

  destroy_configuration(&config);
  return 0;
}
