#pragma once

#ifndef _GC_H_
#define _GC_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

// how many bytes to allocate between GC cycles
#ifndef GC_ALLOC_STEP
#  define GC_ALLOC_STEP 100
#endif

// number of steps to run each cycle
#ifndef GC_STEP_SIZE
#  define GC_STEP_SIZE 5
#endif

/* to be included at beginning of each GC'd object struct */
typedef struct gc_object_header {
  struct gc_object_header *next, *prev;
  bool marked;
} gc_object_header;

struct root {
  struct root* next;
  struct root* prev;

  gc_object_header* object;
};

typedef void* gc_object;
typedef void (*marker)(void*);
typedef void (*sweeper)(void*);

struct gc {
  struct root* roots;

  unsigned allocated;
  bool paused;

  marker mark;
  sweeper sweep;
};

void gc_init(struct gc* g, marker m, sweeper s);
void gc_step(struct gc* g, unsigned steps);
void gc_step_full(struct gc* g);
void gc_add_root(struct gc* g, gc_object_header* object);
void gc_add_object(struct gc* g, gc_object_header* obj);
void gc_add_reference(struct gc* g, gc_object_header* from,
                      gc_object_header* to);
void* gc_alloc(struct gc* g, size_t bytes);
void gc_mark(struct gc* g, gc_object obj);
void gc_pause(struct gc* g);
void gc_unpause(struct gc* g);

#endif /* _GC_H_ */
