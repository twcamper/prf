#ifndef PRF_STACK_H
#define PRF_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "filtered-entry-list.h"

typedef FilteredEntryList Item;

typedef struct prf_stack_type *PrfStack;

PrfStack create_stack(void);
void destroy_stack(PrfStack);
bool push(PrfStack, Item);
Item pop(PrfStack);
void clear_stack(PrfStack);
bool is_empty(PrfStack);
#endif
