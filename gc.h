#pragma once

#ifndef _GC_H_
#define _GC_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "gc_list.h"

typedef bool (*gc_marker)(void*);
typedef void (*gc_sweeper)(void*);

typedef struct gc_list gc_object;

#define SWEEP_LEVEL 100

struct gc {
  struct gc_list *white, *grey, *black, *free;

  unsigned allocated;
  /* floating point to allow e.g. 1 mark every two allocs */
  double to_mark, marks_per_alloc;
  uint8_t paused;

  gc_marker mark;
  gc_sweeper sweep;
};

/* creation and destruction of gc object */
void gc_init(struct gc* g, gc_marker m, gc_sweeper s);
void gc_destroy(struct gc* g);

/* manipulate paused state */
void gc_pause(struct gc* g);
void gc_unpause(struct gc* g);
void gc_resume(struct gc* g);

/* allocate a new object */
gc_object* gc_alloc(struct gc* g, void* object);

/* called from user code to move object into grey list */
void gc_mark(struct gc* g, struct gc_list* obj);

/* deal with roots */
void gc_mark_root(struct gc* g, struct gc_list* obj);
void gc_remove_root(struct gc* g, struct gc_list* obj);
void gc_remove_roots(struct gc* g);

/* garbage collector phases */
void gc_mark_phase(struct gc* g);
void gc_sweep_phase(struct gc* g);

/* force garbage collection cycle */
void gc_collect(struct gc* g);

#define GC_OBJECT(type) typedef struct { \
    GC_LIST_HEADER;                      \
    type* object;                        \
  } GC_##type

#endif /* _GC_H_ */
