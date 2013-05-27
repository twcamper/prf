#include "random-file.h"

static char *find_file(PrfStack s, FilteredEntryList entries, char **ext, size_t extension_count)
{

  static ListItem *entry;
  while ((entry = random_item(entries))) {

    /* get new set of entries if it's another directory */
    if (entry->is_dir) {
      /* save current entry list on the stack */
      push(s, entries);

      /* make a new entry list to search */
      if (!(entries = filtered_entry_list(entry->file_name, ext, extension_count))) {
        fprintf(stderr, "%s:%d %s '%s'\n", __FILE__, __LINE__, "Can't make entry list",  entry->file_name);
        exit(EXIT_FAILURE);
      }

      /* RECURSION */
      return find_file(s, entries, ext, extension_count);
    }
    /* we're done: it's a regular file with a good extension */
    return entry->file_name;
  }

  if (is_empty(s)) {
    /* the stack is empty: we've walked every entire tree under */
    /* the directories in our initial list from config.entries */
    return NULL;
  } else {
    /* we've tried every entry in the current list and it's now empty, */
    /* or it was empty to begin with */
    destroy_list(entries);
    entries = pop(s);
    return find_file(s, entries, ext, extension_count);
  }
}

char *get_random_file(PrfConfig *config)
{
  char *f;
  PrfStack stack = create_stack();
  if (!config->entries) {
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, "NULL initial directory list");
    exit(EXIT_FAILURE);
  }

  srand((unsigned int) time(NULL));
  f = find_file(stack, config->entries, config->ext, config->extension_count);
  destroy_stack(stack);
  return f;
}
