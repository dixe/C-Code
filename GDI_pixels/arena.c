#include "arena.h"

Arena* create_arena(size bytes)
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


// TODO make macro 
void* alloc(Arena* arena, ptrdiff_t objSize, ptrdiff_t align, ptrdiff_t count, i32 flags)
{
  size bytes = objSize * count;

  if (arena->offset + bytes >= arena->cap)
  {
    // out of range
    exit(32);
    return NULL;
  }

  void* p = arena->data + arena->offset;
  arena->offset += bytes;
  return p;
}

void arena_reset(Arena* arena)
{
  memset(arena->data, 0xAB, arena->cap);
  arena->offset = 0;
}