#include "gc.h"

#include <stdlib.h>

// global garbage collector
struct gc _gc;

void gc_init(marker m, sweeper s)
{
  _gc.roots = NULL;
  _gc.allocated = 0;
  _gc.paused = false;
  _gc.mark = m;
  _gc.sweep = s;
}

void gc_pause(void)
{
  _gc.paused = true;
}

void gc_unpause(void)
{
  _gc.paused = false;
}

void gc_step_full(void)
{
  struct root* root = _gc.roots;

  while(root != NULL) {
    _gc.sweep(root->object);
    struct root* next = root->next;
    free(root);

    root = next;
  }
}

void gc_add_root(gc_object_header* object)
{
  struct root* root = calloc(1, sizeof(struct root));
  root->object = object;

  // if this is the first root, assign directly
  if(_gc.roots == NULL) {
    _gc.roots = root;
  }
  // otherwise, insert to head of list
  else {
    root->next = _gc.roots;
    _gc.roots->prev = root;
    _gc.roots = root;
  }
}

// XXX: not even close.
void gc_add_reference(gc_object_header* from, gc_object_header* to)
{
  to->prev->next = to->next;
  to->next->prev = to->prev;

  gc_object_header* next = from->next;
  from->next = to;

  to->next = next;
}

// XXX: steps is unused at this point
void gc_step(unsigned steps)
{
  if(_gc.paused)
    return;

  struct root* root = _gc.roots;

  // mark
  while(root != NULL) {
    _gc.mark(root->object);
  }

  // sweep
  root = _gc.roots;
  while(root != NULL) {
    gc_object_header* obj = root->object;

    while(obj != NULL) {
      if(!obj->marked) {
        gc_object_header
          *next = obj->next,
          *prev = obj->prev;

        _gc.sweep(obj);
        free(obj);

        prev->next = next;
        next->prev = prev;
        obj = next;
      }
    }

    root = root->next;
  }
}

void* gc_alloc(size_t bytes)
{
  _gc.allocated += bytes;

  while(_gc.allocated >= GC_ALLOC_STEP) {
    gc_step(GC_STEP_SIZE);
  }

  gc_object object = NULL;

  /* keep trying to garbage collect on OOM.
     This will probably just cause an infinite loop. */
  while((object = malloc(bytes)) == NULL) {
    gc_step(GC_STEP_SIZE);
  }

  return object;
}
