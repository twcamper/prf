#include "prf-stack.h"

typedef struct node {
  Item value;
  struct node *next;
} Node;

struct prf_stack_type {
  Node *top;
};

static void delete_head(PrfStack);
static void destroy_value(Item);
static void terminate(char *);

PrfStack create_stack(void)
{
  PrfStack s = malloc(sizeof(struct prf_stack_type));
  if (s == NULL)
    terminate("Error in create: stack could not be created.");
  s->top = NULL;
  return s;
}

void destroy_stack(PrfStack s)
{
  clear_stack(s);
  free(s);
}
static void destroy_value(Item i)
{
  destroy_list(i);
}
bool is_empty(PrfStack s)
{
  return s->top == NULL;
}

bool push(PrfStack s, Item i)
{
  Node *new_node;
  if ((new_node = malloc(sizeof(Node))) == NULL)
    return false;
  new_node->value = i;
  /* printf("malloc: %p: %d\n", new_node, new_node->value); */
  new_node->next = s->top;
  s->top = new_node;

  return true;
}

Item pop(PrfStack s)
{
  if (is_empty(s)) {
    terminate("Stack Underflow\n");
  }

  static Item temp_value;
  temp_value = s->top->value;
  delete_head(s);
  return temp_value;
}

void clear_stack(PrfStack s)
{
  while (s->top) {
    destroy_value(s->top->value);
    delete_head(s);
  }
}

static void delete_head(PrfStack s)
{
  /*
   *  if top is not null
   *   assign address of structure pointed to by top to temp_node
   *   assign address of structure pointed to by top->next to top
   *   free temp_node
   */
  Node *temp;
  if (s->top) {
    temp = s->top;
    s->top = s->top->next;
    /* printf("  free: %p: %d\n", temp, temp->value); */
    free(temp);
  }
}
static void terminate(char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}
