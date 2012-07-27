#include <stdio.h>

#include "gc.h"
#include "gc_list.h"
#include "debug.h"

/* internal gc functions */
static inline void gc_mark_init(struct gc* g);
static void gc_sweep_list(struct gc* g, struct gc_list* list,
                          enum gc_object_color color);
static void gc_mark_grey(struct gc* g, unsigned num);
static inline void gc_mark_greys(struct gc* g);

void gc_init(struct gc* g, gc_marker m, gc_sweeper s)
{
  g->white = list_create(GC_WHITE);
  g->grey  = list_create(GC_GREY);
  g->black = list_create(GC_BLACK);
  g->free  = list_create(GC_FREE);

  list_loop(g->white);
  g->white->prev = g->free;
  g->white->next = g->grey;
  g->grey->prev = g->white;
  g->grey->next = g->black;
  g->black->prev = g->grey;
  g->black->next = g->free;
  g->free->prev = g->black;
  g->free->next = g->white;

  g->allocated = 0;
  g->to_mark = 0;
  g->marks_per_alloc = .5;

  g->paused = 0;

  g->mark = m;
  g->sweep = s;
}

void gc_destroy(struct gc* g)
{
  gc_sweep_list(g, g->white, GC_WHITE);
  gc_sweep_list(g, g->grey, GC_GREY);
  gc_sweep_list(g, g->black, GC_BLACK);

  struct gc_list* f = g->free;
  while(f) {
    struct gc_list* next = f->next;
    free(f);
    f = next;
  }

  free(g);
}

void gc_pause(struct gc* g)
{
  g->paused++;
}

void gc_unpause(struct gc* g)
{
  g->paused--;
}

void gc_resume(struct gc* g)
{
  g->paused = 0;
}

gc_object* gc_alloc(struct gc* g, void* object)
{
  LOG("gc alloc");
  struct gc_list* obj = g->free->next;

  if(obj->color != GC_FREE) {
    LOGF("creating new element\n");
    obj = list_create(GC_WHITE);
  }

  obj->object = object;

  LOG("moving new object to white");
  list_move(obj, g->white, GC_WHITE);

  g->allocated++;
  g->to_mark += g->marks_per_alloc;

  if(!g->paused) {

    if(g->to_mark > 1.0) {
      // explicitly place into black list to prevent being freed
      // before being returned
      obj->color = GC_BLACK;
      list_move(obj, g->black, GC_BLACK);

      gc_mark_phase(g);
    }
  }
  return obj;
}

void gc_mark(struct gc* g, struct gc_list* obj)
{
  if(obj->color == GC_WHITE)
    list_move(obj, g->grey, GC_GREY);
}

void gc_mark_root(struct gc* g, struct gc_list* obj)
{
  if(obj->root) return;

  obj->root = true;
  list_move(obj, g->grey, GC_GREY);
}

void gc_remove_root(struct gc* g, struct gc_list* obj)
{
  assert(obj->root);

  obj->root = false;
  list_move(obj, g->white, GC_WHITE);
}

void gc_mark_phase(struct gc* g)
{
  if(g->allocated > SWEEP_LEVEL)
    gc_sweep_phase(g);
  else
    gc_mark_grey(g, (unsigned)g->to_mark);

  // grey list is empty
  if(g->grey->next->color != GC_GREY) {
    gc_sweep_list(g, g->white, GC_WHITE);
  }
}

void gc_sweep_phase(struct gc* g)
{
  gc_mark_greys(g);

  gc_sweep_list(g, g->white, GC_WHITE);

  // swap black and white lists
  struct gc_list* tmp = g->white;
  g->white = g->black;
  g->black = tmp;

  // place root objects back into grey list
  gc_mark_init(g);
}

void gc_collect(struct gc* g)
{
  assert(!g->paused);

  gc_sweep_phase(g);
}

static inline void gc_mark_init(struct gc* g)
{
  /* start by moving all root objects back into the grey list */
  struct gc_list* list = g->white;

  while(list != NULL && list->color == GC_WHITE) {
    struct gc_list* next = list->next;
    if(list->root)
      list_move(list, g->grey, GC_GREY);
    list = next;
  }
}

static void gc_sweep_list(struct gc* g, struct gc_list* list,
                          enum gc_object_color color)
{
  unsigned frees = 0;
  LOGF("looking for color %d\n", color);

  while(list != NULL && list->color == color) {
    struct gc_list* next = list->next;

    LOGF("%p, %p\n", (void*)list, (void*)next);

    g->sweep(list->object);
    frees++;

    list->object = NULL;
    list_move(list, g->free, GC_FREE);

    list = next;
  }

  LOGF("freed %d from list %d\n", frees, color);

  g->allocated -= frees;
}

static void gc_mark_grey(struct gc* g, unsigned num)
{
  struct gc_list *ptr = g->grey, *next = NULL;
  while(ptr != NULL && ptr->color == GC_GREY) {
    next = ptr->next;

    LOGF("%p, %p\n", (void*)ptr, (void*)next);

    if(ptr->root || g->mark(ptr->object)) {
      list_move(ptr, g->black, GC_BLACK);
    }
    ptr = next;
  }

  g->to_mark = 0;
}

static inline void gc_mark_greys(struct gc* g)
{
  gc_mark_grey(g, 10000);
}
