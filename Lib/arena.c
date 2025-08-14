#include "arena.h"

Arena* arena_create(isize bytes)
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
  isize available = arena->cap - arena->offset - padding - bytes;
  if (available < 0 )
  {
    // oom
    exit(32);
    return NULL;
  }

  void* p = arena->data + arena->offset + padding;
  arena->offset += bytes + padding;;
  return p;
}



void* arena_realloc(Arena* arena, u8* ptr, isize current_size, ptrdiff_t new_size)
{
  isize align = _Alignof(u8);
  isize padding = -(isize)(arena->data + arena->offset) & (align - 1);

  isize old_end = (isize)ptr + current_size;
  isize arena_end = (isize)arena->data + arena->offset;


  isize available = arena->cap - arena->offset - padding - new_size;
  if (available < 0)
  {
    // oom
    exit(32);
    return NULL;
  }

  if (old_end == arena_end )
  {
    arena->offset += new_size - current_size;
    return ptr;
  }

  return alloc(arena, sizeof(u8), _Alignof(u8), new_size - current_size, 0);
  
}

void arena_reset(Arena* arena)
{
#ifdef _DEBUG
  memset(arena->data, 0xAB, arena->cap);
#endif
  arena->offset = 0;
}