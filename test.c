#include <stdio.h>
#include <stdlib.h>

#include "gc.h"

struct gc gc;

typedef struct ListNode {
  struct ListNode* next;
  int value;
} ListNode;

GC_OBJECT(ListNode);

bool should_mark = true;

bool mark_node(void* obj)
{
  if(!obj) {
    puts(">> NOT marking NULL node");
    return false;
  }

  ListNode* node = (ListNode*)obj;

  printf(">> Marking node for collection: %d\n", node->value);
  return false;
}

void sweep_node(void* obj)
{
  if(!obj) {
    puts(">> Sweeping NULL node");
    return;
  }
  ListNode* node = (ListNode*)obj;
  printf(">> Sweeping node %d\n", node->value);
  free(node);
}

int main(int argc, char** argv)
{
  gc_init(&gc, mark_node, sweep_node);

  for(unsigned i = 0; i < 10; ++i) {
    ListNode* node = malloc(sizeof(ListNode));
    node->value = i;
    gc_alloc(&gc, node);
  }

  // everything should be collected
  gc_collect(&gc);

  return 0;
}
