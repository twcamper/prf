# tested with GNU make 3.81
SHELL   = /usr/bin/env sh
CC      = clang
CFLAGS  = -g  -Wall -Wextra -pedantic -std=c99
LD      = clang

#### targets and prerequisites ####
TEMP        = $(shell find . -name '*.c' |  tr '\n' ' ')
OBJECTS     = $(TEMP:.c=.o)
EXECUTABLES = $(TEMP:.c=))

bin/prf : % : src/prf.o
	$(LD) $< -o $@

#### compiled object files ####
$(OBJECTS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY : clean clean-obj clean-bin

clean: clean-obj clean-bin

XARGS_RM = xargs rm -fv

clean-obj:
	@find . -name '*.o' | $(XARGS_RM)

clean-bin:
	@find . -perm +111 -type f | grep -vE "\.(git|sh|rb)" | $(XARGS_RM)
