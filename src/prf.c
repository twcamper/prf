#include "prf-config.h"
#include "random-file.h"
#include "played.h"

int main(int argc, char *argv[])
{
  PrfConfig config = read_configuration(&argc, &argv);
  char *file  =  get_random_file(&config);
  printf("%s\n", file);
  if (log_as_played(file, config.log_file) != 0)
    fprintf(stderr, "Error logging '%s' to '%s'\n", file, config.log_file);

  destroy_configuration(&config);
  return 0;
}
