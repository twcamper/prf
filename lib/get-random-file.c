#include "get-random-file.h"

static bool can_play(char *ext[], size_t n, char *filename)
{
  char *p;
  for (size_t i = 0; i < n; i++)
    if ((p = strrchr(filename, '.')))
      if (strcmp(++p, ext[i]) == 0)
        return true;

  return false;
}
static char *find_file(FilteredEntryList entries, char **ext, size_t extension_count)
{
  ListItem *entry;
  while ((entry = random_item(entries))) {

    /* get new set of entries if it's another directory */
    if (entry->is_dir) {
      if (!(entries = filtered_entry_list(entry->file_name, ext))) {
        fprintf(stderr, "%s:%d %s '%s'\n", __FILE__, __LINE__, "Can't make entry list",  entry->file_name);
        exit(EXIT_FAILURE);
      }

      /* RECURSION */
      return find_file(entries, ext, extension_count);
    }
    /* we're done: it's a regular file with a good extension */
    return entry->file_name;
  }
  return NULL;
}

char *get_random_file(PrfConfig config)
{
  srand((unsigned int) time(NULL));
  return find_file(config.entries, config.ext, config.extension_count);
}
