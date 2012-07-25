#pragma once

#ifndef _GC_LIST_H_
#define _GC_LIST_H_

/* implementation of a circular doubly linked list */

enum gc_object_color {
  GC_WHITE,
  GC_GREY,
  GC_BLACK,
  GC_FREE
};

#define GC_LIST_HEADER   \
  struct gc_list* next;  \
  struct gc_list* prev;  \
  unsigned color : 3;    \
  unsigned root  : 1

struct gc_list {
  GC_LIST_HEADER;
  void* object;
};

struct gc_list* list_create(enum gc_object_color color);
void list_destroy(struct gc_list* list);
void list_loop(struct gc_list* list);
void list_remove(struct gc_list* list);
void list_move(struct gc_list* from_list, struct gc_list* to_list,
  enum gc_object_color color);

#endif /* _GC_LIST_H_ */
