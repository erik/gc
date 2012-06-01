#include <stdio.h>
#include <stdlib.h>

#include "gc.h"

struct gc gc;

typedef struct ListNode {
  struct ListNode* next;
  int value;
} ListNode;

void mark_node(gc_object obj)
{
  ListNode* node = (ListNode*)obj;

  printf("Marking node %d\n", node->value);

  gc_mark(&gc, obj);

  if(node->next)
    mark_node((void*)node->next);
}

void sweep_node(gc_object obj)
{
  ListNode* node = (ListNode*)obj;
  printf("Freeing node %d\n", node->value);
}

int main(int argc, char** argv)
{
  gc_init(&gc, mark_node, sweep_node);

  ListNode* root = gc_alloc(&gc, sizeof(ListNode));
  root->value = 2;

  gc_add_root(&gc, (void*)root);

  ListNode* node = root;
  for(int i = 0; i < 4; ++i) {
    printf("Allocating node %d\n", i);

    ListNode* next = gc_alloc(&gc, sizeof(ListNode));
    next->value = i;
    next->next = NULL;

    node->next = next;
    node = next;

  }

  // free everything
  gc_step_full(&gc);

  return 0;
}
