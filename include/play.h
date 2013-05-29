#ifndef PLAY_H
#define PLAY_H

#include "prf-config.h"
#include <errno.h>
#include <sys/wait.h>

int play(char *file, char *player);
char *get_player(char *ext, PrfConfig *);
char *get_extension(char *file);
int kill_all(char *player);
#endif
