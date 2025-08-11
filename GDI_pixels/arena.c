#include "arena.h"

Arena* create_arena(isize bytes)
{
  Arena* a;
  a = (Arena*)malloc(sizeof(a));
  if (a == NULL) {
    exit(29);
    return NULL;
  }
  char* data = malloc(bytes);
  if (data == NULL)
  {    
    exit(30);  
    return NULL;
  }

  a->data = data;
  a->offset = 0;
  a->cap = bytes;
  memset(a->data, 0xAB, a->cap);
  return a;
}


void* alloc(Arena* arena, ptrdiff_t objSize, ptrdiff_t align, ptrdiff_t count, i32 flags)
{
  isize bytes = objSize * count;

  isize padding = -(isize)(arena->data + arena->offset) & (align - 1);
  isize available = arena->cap - arena->offset - padding;
  if (available < 0)
  {
    // oom
    exit(32);
    return NULL;
  }

  void* p = arena->data + arena->offset + padding;
  arena->offset += bytes + padding;;
  return p;
}

void arena_reset(Arena* arena)
{
  memset(arena->data, 0xAB, arena->cap);
  arena->offset = 0;
}