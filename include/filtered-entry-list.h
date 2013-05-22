#ifndef FILTERED_ENTRY_LIST_H
#define FILTERED_ENTRY_LIST_H

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

typedef struct list_item {
  bool is_dir;
  char file_name[];
} ListItem;

typedef struct filtered_entry_list_type *FilteredEntryList;

FilteredEntryList filtered_entry_list(char *, char **);
ListItem *random_item(FilteredEntryList);
void destroy_list(FilteredEntryList);
void print_list(FilteredEntryList);
#endif
