#include "arena.h"

#ifndef ARENA_RESERVE_SIZE
#define ARENA_RESERVE_SIZE  ((i64)1) << 10
#endif 
//
//#define MEM_RESERVE 0x00002000
//#define MEM_COMMIT  0x00001000
//#define PAGE_READWRITE 0x04


Arena arena_create(isize bytes)
{
  Arena a = { 0 };

  SIZE_T reserve = ARENA_RESERVE_SIZE ;
  // reserve large continious space
  void* reserved = VirtualAlloc(NULL, reserve, MEM_RESERVE, PAGE_READWRITE);

  // commit required bytes
  u8* data = (u8*)VirtualAlloc(reserved, bytes, MEM_COMMIT, PAGE_READWRITE);
  
  if (data == NULL)
  {    
    u64 errNo = GetLastError();
    exit(30);  
    return a;
  }

  a.data = data;
  a.offset = 0;
  a.cap = bytes;
  a.flags |= ARENA_GROWABLE;
  memset(a.data, 0xAB, a.cap);
  return a;
}


Arena arena_create_fixed(u8* data, isize bytes)
{
  Arena a = { 0 };
  a.data = data;
  a.offset = 0;
  a.cap = bytes;
  a.flags |= ARENA_FIXED;
  memset(a.data, 0xF0, a.cap);
  return a;
}


void* alloc(Arena* arena, ptrdiff_t objSize, ptrdiff_t align, ptrdiff_t count, i32 flags)
{
  isize bytes = objSize * count;


  isize padding = -(isize)(arena->data + arena->offset) & (align - 1);
  isize available = arena->cap - arena->offset - padding - bytes;
  if (available < 0 )
  {
    if (arena->flags & ARENA_FIXED)
    {
      // OOM in fixed return void and exit

      exit(9);
      return 0;
    }
    // commit more from reserved space. Assume oom is fatal
    u8* new_pointer = (u8*)VirtualAlloc(arena->data + arena->cap, bytes, MEM_COMMIT, PAGE_READWRITE);
    arena->cap = bytes;

    memset(arena->data + arena->offset, 0xAC, bytes);
    if (new_pointer == NULL)
    {
      // oom
      exit(32);
      return NULL;
    }
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

  isize required = new_size + padding;
  isize available = arena->cap - arena->offset - padding - new_size;
  if (available < 0)
  {
    if (arena->flags & ARENA_FIXED )
    {
      // OOM in fixed return void and exit

      exit(9);
      return 0;
    }
    
    u8* new_pointer = (u8*)VirtualAlloc(arena->data + arena->cap, required, MEM_COMMIT, PAGE_READWRITE);
    if (new_pointer == NULL)
    {
      // oom
      exit(32);
      return NULL;
    }

    arena->cap = arena->cap + required;
    memset(arena->data + arena->offset, 0xAD, required);
    if (new_pointer == NULL)
    {
      // oom
      exit(32);
      return NULL;
    }
  }

  if (old_end == arena_end )
  {
    arena->offset += required - current_size;
    return ptr;
  }

  return alloc(arena, sizeof(u8), _Alignof(u8), required, 0);
  
}

void arena_reset(Arena* arena)
{
#ifdef _DEBUG
  memset(arena->data, 0xAB, arena->cap);
#endif
  arena->offset = 0;
}