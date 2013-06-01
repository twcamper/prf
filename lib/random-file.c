#include "random-file.h"

static char *find_file(PrfStack s, char *log_file,  FilteredEntryList entries, char **ext, size_t extension_count)
{
  bool rc;
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
      return find_file(s, log_file,  entries, ext, extension_count);
    }
    if (!(rc = has_been_played(entry->file_name, log_file))) {
      /* we're done: it's a regular file with a good extension */
      destroy_list(entries);
      return entry->file_name;
    } else if (rc == -1)
      /* IO error */
      return NULL;
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
    return find_file(s, log_file, entries, ext, extension_count);
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
  f = find_file(stack, config->log_file, config->entries, config->ext, config->extension_count);
  destroy_stack(stack);
  return f;
}
