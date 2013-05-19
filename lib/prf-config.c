#include "prf-config.h"

PrfConfig read_configuration(int argc, char *argv[])
{
  PrfConfig prf_config = {
    1,
    {"flac"},
    4,
    {"/Users/timothycamper/Music/baroque",
     "/Users/timothycamper/Music/rock",
     "/Users/timothycamper/Music/jazz",
     "/Users/timothycamper/Music/soul"}};

  if (argc > 0)
    argv[1] = argv[1]; /* hoping to shut up clang for now */

  return prf_config;
}
