#include "filtered-entry-list.h"

struct filtered_entry_list_type {
  size_t allocated;
  size_t available;
  ListItem *entries[];
};

static void print_error(char *f, long l, char *msg)
{
  fprintf(stderr, "%s:%ld \t", f, l);
  perror(msg);
}
FilteredEntryList vector_to_filtered_entry_list(char **paths, size_t count)
{
  if (!paths) {
    print_error(__FILE__, __LINE__, "NULL paths list");
    exit(EXIT_FAILURE);
  }

  struct stat fs;
  static FilteredEntryList list;
  if (!(list = malloc(sizeof(struct filtered_entry_list_type) +
                      (count * sizeof(ListItem *))))) {
    print_error(__FILE__, __LINE__, "malloc() of entries list");
    exit(EXIT_FAILURE);
  }
  list->allocated = 0;
  for (size_t i = 0; i < count; i++) {
    /* stat the file */
    errno = 0;
    if (stat(paths[i], &fs)) {
      print_error(__FILE__, __LINE__, paths[i]);
      exit(EXIT_FAILURE);
    }
    /* skip but report if it's a funny type */
    if (!(S_ISDIR(fs.st_mode) || S_ISREG(fs.st_mode))) {
      fprintf(stderr, "%s:%d %s is neither a directory or regular file\n", 
          __FILE__, __LINE__, paths[i]);
      continue;
    }
    /* store a new list item in the new slot in the entries list */
    if (!(list->entries[list->allocated] = malloc(sizeof(ListItem) +
            (sizeof(char) * (strlen(paths[i]) + 1))))) {
      print_error(__FILE__, __LINE__, "malloc() on ListItem");
      exit(EXIT_FAILURE);
    }

    /* populate the new item */
    strcpy(list->entries[list->allocated]->file_name, paths[i]);
    list->entries[list->allocated]->is_dir = false;
    if (S_ISDIR(fs.st_mode))
      list->entries[list->allocated]->is_dir = true;

    /* count the allocation */
    list->allocated++;

  }
  list->available = list->allocated;
  return list;
}
static bool has_good_extension(char *f, char *ext[], size_t extension_count)
{
  char *p = strrchr(f, '.');
  if (p) {
    p++;  /* char after dot */
    for (size_t i = 0; i < extension_count; i++) {
      if (strcmp(p, ext[i]) == 0)
        return true;
    }
  }

  return false;
}

FilteredEntryList filtered_entry_list(char *dir, char **ext, size_t extension_count)
{
  DIR *dir_ptr;
  static FilteredEntryList list;
  struct dirent *entry_ptr;
  struct stat fs;
  char path[FILENAME_MAX];

  if (!dir) {
    print_error(__FILE__, __LINE__, "dir parameter is NULL");
    return NULL;
  }

  /* Open the Directory */
  errno = 0;
  if ((dir_ptr = opendir(dir)) == NULL) {
    print_error(__FILE__, __LINE__, dir);
    return NULL;
  }
  /* store absolute path in our temp buffer, 
   * which will begin each entry below
   */
  strcpy(path, dir);
  size_t pathlen = strlen(path);
  if (path[pathlen - 1] != '/') {
    strcat(path, "/");
    pathlen++;
  }
  if (!(list = malloc(sizeof(struct filtered_entry_list_type))))
    print_error(__FILE__, __LINE__, "initial malloc() of entries list");

  list->allocated = 0;

  while  ((entry_ptr = readdir(dir_ptr))) {
    /* ignore self and parent */
    if (strcmp(entry_ptr->d_name, ".") == 0 || strcmp(entry_ptr->d_name, "..") == 0)
      continue;

    /* build absolute path to entry */
    path[pathlen] = '\0';
    strcat(path, entry_ptr->d_name);

    /* stat the file */
    errno = 0;
    if (stat(path, &fs)) {
      print_error(__FILE__, __LINE__, path);
      continue;  /* try the next one upon error */
    }

    /* filter the file */
    if ( S_ISDIR(fs.st_mode) ||
        (S_ISREG(fs.st_mode) && has_good_extension(path, ext, extension_count))) {

      /* grow the entries list to hold another item pointer */
      if (!(list = realloc(list,
                           sizeof(struct filtered_entry_list_type) +
                           ((list->allocated + 1) * sizeof(ListItem *)))))
        print_error(__FILE__, __LINE__, "realloc() on entries list");

      /* store a new list item in the new slot in the entries list */
      if (!(list->entries[list->allocated] = malloc(sizeof(ListItem) +
                                       (sizeof(char) * (strlen(path) + 1)))))
        print_error(__FILE__, __LINE__, "malloc() on ListItem");

      /* populate the new item */
      strcpy(list->entries[list->allocated]->file_name, path);
      list->entries[list->allocated]->is_dir = false;
      if (S_ISDIR(fs.st_mode))
        list->entries[list->allocated]->is_dir = true;

      /* count the allocation */
      list->allocated++;
    }
  }

  /* set count fields now that the struct is created */
  list->available = list->allocated;

  /* Close the Directory */
  errno = 0;
  if (closedir(dir_ptr) != 0) {
    print_error(__FILE__, __LINE__, dir);
    return NULL;
  }
  return list;
}

void print_list(FilteredEntryList list)
{
  size_t i;
  printf("Size: %ld\n", list->available);
  for (i = 0; i < list->available; i++)
    printf("%p] %d: %s\n", &list->entries[i], list->entries[i]->is_dir, list->entries[i]->file_name);
}

void destroy_list(FilteredEntryList list)
{
  for (size_t i = 0; i < list->allocated; i++)
    free(list->entries[i]);
  free(list);
}
static ListItem *remove_item(FilteredEntryList list, size_t i)
{
  static ListItem *temp;

  /* out of range or empty list */
  if (i >= list->available)
    return NULL;

  /* last item, no swap needed */
  if (i == list->available - 1) {
    list->available--;
    return list->entries[i];
  }

  /* "remove" by swapping pointers: target pointer is copied, */
  /* then moved to the end */
  temp = list->entries[i];
  list->entries[i] = list->entries[list->available - 1];
  list->entries[list->available - 1] = temp;
  list->available--;

  return temp;
}
ListItem *random_item(FilteredEntryList list)
{
  /* Depends on srand() being called by the client */
  if (list->available)
    return remove_item(list, rand() % list->available);
  return NULL;
}
