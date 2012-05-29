#pragma once

#ifndef _GC_H_
#define _GC_H_

#include <stddef.h>
#include <stdbool.h>

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
typedef int (*marker)(gc_object_header*);
typedef void (*sweeper)(gc_object_header*);

struct gc {
  struct root* roots;

  unsigned allocated;
  bool paused;

  marker mark;
  sweeper sweep;
};

// global garbage collector
extern struct gc _gc;

void gc_init(marker m, sweeper s);

void gc_step(unsigned steps);
void gc_step_full(void);

void gc_add_root(gc_object_header* object);
void gc_add_reference(gc_object_header* from, gc_object_header* to);
void* gc_alloc(size_t bytes);

void gc_pause(void);
void gc_unpause(void);

#endif /* _GC_H_ */
