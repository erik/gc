//#define NDEBUG 1

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "gc_list.h"

struct gc_list* list_create(enum gc_object_color color)
{
  struct gc_list* list = malloc(sizeof(struct gc_list));
  list->next = NULL;
  list->prev = NULL;

  list->color = color;
  list->root = false;

  list->object = NULL;

  return list;
}

void list_destroy(struct gc_list* list)
{
  struct gc_list* ptr = list;
  do
    free(ptr);
  while((ptr = ptr->next));
}

void list_loop(struct gc_list* list)
{
  list->next = list;
  list->prev = list;
}

void list_remove(struct gc_list* obj)
{
  obj->prev->next = obj->next;
  obj->next->prev = obj->prev;
}

void list_move(struct gc_list* from_list, struct gc_list* to_list,
               enum gc_object_color color)
{
  if(from_list->prev)
    list_remove(from_list);

  from_list->color = color;

  from_list->next = to_list->next;
  from_list->prev = to_list;

  to_list->next->prev = from_list;
  to_list->next = from_list;
}
