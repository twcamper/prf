#include "prf-config.h"
#include "get-random-file.h"

int main(int argc, char *argv[])
{
  PrfConfig config = read_configuration(&argc, &argv);
  puts(get_random_file(config));
  destroy_configuration(&config);
  return 0;
}
