#ifndef PLAY_H
#define PLAY_H

#include "prf-config.h"
#include <errno.h>
#include <sys/wait.h>
#include <FLAC/format.h>
#include <FLAC/metadata.h>

int play(char *file, char *player);
char *get_player(char *ext, PrfConfig *);
char *get_extension(char *file);
int kill_all(char *player);
time_t get_duration(char *file, char *ext);
#endif
