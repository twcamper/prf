#ifndef PLAY_H
#define PLAY_H

#include "prf-config.h"
#include <errno.h>

int play(char *file, char *player);
char *get_player(char *ext, PrfConfig *);
char *get_extension(char *file);
#endif
