#include "gc.h"

void gc_init(struct gc* g, marker m, sweeper s)
{
  g->roots = NULL;
  g->allocated = 0;
  g->paused = false;
  g->mark = m;
  g->sweep = s;
}

void gc_pause(struct gc* g)
{
  g->paused = true;
}

void gc_unpause(struct gc* g)
{
  g->paused = false;
}

void gc_step_full(struct gc* g)
{
  struct root* root = g->roots;

  while(root != NULL) {
    g->sweep(root->object);
    struct root* next = root->next;
    free(root);

    root = next;
  }
}

void gc_add_root(struct gc* g, gc_object_header* object)
{
  struct root* root = calloc(1, sizeof(struct root));
  root->object = object;

  // if this is the first root, assign directly
  if(g->roots == NULL) {
    g->roots = root;
  }
  // otherwise, insert to head of list
  else {
    root->next = g->roots;
    g->roots->prev = root;
    g->roots = root;
  }
}

void gc_add_object(struct gc* g, gc_object_header* object)
{
  // TODO
}

// XXX: not even close.
void gc_add_reference(struct gc* g, gc_object_header* from,
                      gc_object_header* to)
{
  to->prev->next = to->next;
  to->next->prev = to->prev;

  gc_object_header* next = from->next;
  from->next = to;

  to->next = next;
}

// XXX: steps is unused at this point
void gc_step(struct gc* g, unsigned steps)
{
  if(g->paused)
    return;

  struct root* root = g->roots;

  // mark
  while(root != NULL) {
    g->mark(root->object);
    root = root->next;
  }

  // sweep
  root = g->roots;
  while(root != NULL) {
    gc_object_header* obj = root->object;

    while(obj != NULL) {
      if(!obj->marked) {
        gc_object_header
          *next = obj->next,
          *prev = obj->prev;

        g->sweep(obj);
        free(obj);

        prev->next = next;
        next->prev = prev;
        obj = next;
      } else {
        obj = obj->next;
      }
    }

    root = root->next;
  }
}

void* gc_alloc(struct gc* g, size_t bytes)
{
  while(g->allocated >= GC_ALLOC_STEP) {
    gc_step(g, GC_STEP_SIZE);
  }

  gc_object object = NULL;
  g->allocated += bytes;

  object = malloc(bytes);

  return object;
}
