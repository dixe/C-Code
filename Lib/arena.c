#include "arena.h"
#include <basetsd.h>
#include <stdlib.h>
#include <memory.h>


#ifndef ARENA_RESERVE_SIZE
#define ARENA_RESERVE_SIZE  ((i64)1) << 40
#endif 
#define ARENA_SET_MEM_DEBUG_NOT
//
#define MEM_RESERVE 0x00002000
#define MEM_COMMIT  0x00001000
#define PAGE_READWRITE 0x04

typedef unsigned long DWORD;
typedef void* LPVOID;

LPVOID VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD  flAllocationType, DWORD  flProtect);

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
    exit(30);  
    return a;
  }

  a.data = data;
  a.offset = 0;
  a.cap = bytes;
  a.flags |= ARENA_GROWABLE;
#ifdef ARENA_SET_MEM_DEBUG
  memset(a.data, 0xAB, a.cap);
#endif // ARENA_SET_MEM_DEBUG

  
  return a;
}


Arena arena_create_fixed(u8* data, isize bytes)
{
  Arena a = { 0 };
  a.data = data;
  a.offset = 0;
  a.cap = bytes;
  a.flags |= ARENA_FIXED;
#ifdef ARENA_SET_MEM_DEBUG
  memset(a.data, 0xF0, a.cap);
#endif
  return a;
}


void* alloc(Arena* arena, ptrdiff_t objSize, ptrdiff_t align, ptrdiff_t count, i32 flags)
{
  isize padding = -(isize)(arena->data + arena->offset) & (align - 1);
  isize requested_bytes = objSize * count + padding;
  isize bytes_to_alloc = requested_bytes - (arena->cap - arena->offset);
  if (bytes_to_alloc > 0 )
  {
    if (arena->flags & ARENA_FIXED)
    {
      // OOM in fixed return void and exit

      exit(9);
      return 0;
    }
    // commit more from reserved space. Assume oom is fatal
    u8* new_pointer = (u8*)VirtualAlloc(arena->data + arena->cap, bytes_to_alloc, MEM_COMMIT, PAGE_READWRITE);
    if (new_pointer == NULL)
    {
      // oom
      exit(32);
      return NULL;
    }

    arena->cap += bytes_to_alloc;    
    memset(arena->data + arena->offset, 0xAC, bytes_to_alloc);
  }
  void* p = arena->data + arena->offset + padding;
  // increment offset to reserve the allocation
  arena->offset += requested_bytes;
  if (arena->offset > arena->cap)
  {
    // OOM in fixed return void and exit

    exit(10);
    return 0;
  }
  return p;
}



void* arena_realloc(Arena* arena, u8* ptr, isize current_size, ptrdiff_t new_size)
{
  isize align = _Alignof(u8);
  isize padding = -(isize)(arena->data + arena->offset) & (align - 1);

  // if ptr + current size == arena->offset, we can just allocate at the end
  if (arena->data + arena->offset == ptr + current_size)
  {
    // simply alloc new_size - current_size and return current pointer
    alloc(arena,sizeof(u8), _Alignof(u8), new_size - current_size, 0);
    return ptr;
  }

  // new allocation has to be the full newsize
  return alloc(arena, sizeof(u8), _Alignof(u8), new_size, 0);
}

void arena_reset(Arena* arena)
{
#ifdef ARENA_SET_MEM_DEBUG
  memset(arena->data, 0xAB, arena->cap);
#endif
  arena->offset = 0;
}