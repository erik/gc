#include <stdio.h>
#include <stdlib.h>

#include "gc.h"

typedef struct ListNode {
  gc_object_header* obj;
  struct ListNode* next;
  int value;
} ListNode;

void mark_node(gc_object_header* obj)
{
  ListNode* node = (ListNode*)obj;

  obj->marked = true;
  if(node->next)
    mark_node((void*)node->next);
}

void sweep_node(gc_object_header* obj)
{
  ListNode* node = (ListNode*)obj;
  printf("Freeing node %d\n", node->value);
  free(node);
}

int main(int argc, char** argv)
{
  struct gc _gc;
  struct gc* gc = &_gc;

  gc_init(gc, mark_node, sweep_node);

  ListNode* root = gc_alloc(gc, sizeof(ListNode));
  root->value = 2;

  gc_add_root(gc, (void*)root);

  ListNode* node = root;
  for(int i = 0; i < 4; ++i) {
    printf("Allocating node %d\n", i);

    ListNode* next = gc_alloc(gc, sizeof(ListNode));
    next->next = NULL;

    node->next = next;
    node = next;
  }

  // free everything
  gc_step_full(gc);

  return 0;
}
