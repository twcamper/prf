#ifndef PRF_STACK_H
#define PRF_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <glob.h>

struct visited_dir {
  glob_t *entries;
  size_t tried_count;
  size_t **tried;
};

typedef struct visited_dir Item;

typedef struct prf_stack_type *PrfStack;

PrfStack create_stack(void);
void destroy_stack(PrfStack);
bool push(PrfStack, Item);
Item pop(PrfStack);
void clear_stack(PrfStack);
bool is_empty(PrfStack);
#endif
