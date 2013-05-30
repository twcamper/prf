#ifndef PLAY_H
#define PLAY_H

#include "prf-config.h"
#include <FLAC/format.h>
#include <FLAC/metadata.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

int play(char *file, char *player);
char *get_player(char *ext, PrfConfig *);
char *get_extension(char *file);
int kill_all(char *player);
time_t get_duration(char *file, char *ext);
int register_signal_handler(int signal, void (*handler)(int));
void kill_player(int signal);
#endif
