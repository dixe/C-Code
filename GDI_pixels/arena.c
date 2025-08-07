#include "arena.h"

Arena* create_arena(int bytes)
{
  Arena* a;
  a = (Arena*)malloc(sizeof(a));
  void* data = malloc(bytes);
  if (data == NULL)
  {
    exit(30);      // TODO abort or exit
  }

  a->data = (char*)data;
  a->offset = 0;
  a->size = bytes;
  memset(a->data, 0xAB, a->size);
  return a;
}


// TODO make macro 
void* arena_alloc(Arena* arena, int  bytes) {

  void* p = arena->data + arena->offset;
  arena->offset += bytes;
  return p;
}

void arena_reset(Arena* arena)
{
  memset(arena->data, 0xAB, arena->size);
  arena->offset = 0;
}