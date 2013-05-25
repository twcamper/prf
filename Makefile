# tested with GNU make 3.81
SHELL   = /usr/bin/env sh
CC      = clang
LD      = clang
PREFIX  = ./
LIBS    = $(PREFIX)lib
INCS    = $(PREFIX)include
CFLAGS  = -g  -Wall -Wextra -pedantic -std=c99 -I$(INCS)

all: bin/prf

bin/prf : % : src/prf.o \
	            $(LIBS)/get-random-file.o \
							$(LIBS)/prf-config.o \
							$(LIBS)/filtered-entry-list.o \
							$(LIBS)/prf-stack.o
	$(LD) $^ -o $@

src/prf.o: %.o : %.c \
	               $(INCS)/get-random-file.h \
								 $(INCS)/prf-config.h \
								 $(INCS)/filtered-entry-list.h \
								 $(INCS)/prf-stack.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY : clean clean-obj clean-bin

clean: clean-obj clean-bin

XARGS_RM = xargs rm -fv

clean-obj:
	@find . -name '*.o' | $(XARGS_RM)

clean-bin:
	@find . -perm +111 -type f | grep -vE "\.(git|sh|rb)" | $(XARGS_RM)

include $(LIBS)/makefile
