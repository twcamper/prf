#include "get-random-file.h"
#include "prf-config.h"

int main(int argc, char *argv[])
{
  PrfConfig config = read_configuration(argc, argv);
  puts(get_random_file(config.path, config.ext));
  return 0;
}
