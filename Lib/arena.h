#pragma once

#include "custom_types.h"

typedef enum 
{
  ARENA_FIXED = 1,
  ARENA_GROWABLE = 1 << 1,
} ARENA_FLAGS;

typedef struct {
  isize cap;
  isize offset;
  u8* data;
  ARENA_FLAGS flags;
} Arena;



Arena arena_create(isize bytes);

Arena arena_create_fixed(u8* data, isize bytes);

void* alloc(Arena* a, ptrdiff_t isize, ptrdiff_t align, ptrdiff_t count, i32 flags);

void arena_reset(Arena* arena);

/// <summary>
/// Try to reallocate a pointer with current_size, to a new pointer with new_size
/// If the pointer is at the end of the arena, the arena is extended, and the current pointer is returned
/// If the pointer could not be extended, a new pointer with new_size is returned, the old pointer is stil valid
/// Realloc does not do any memcopy of the reallocated pointer
/// </summary>
/// <param name="arena">Arena that the current allocation is in and where the new allocation will be</param>
/// <param name="ptr">Pointer to current allocaiton</param>
/// <param name="current_size">Size in bytes of current allocation</param>
/// <param name="new_size">Size in bytes of new allocatio</param>
/// <returns></returns>
void* arena_realloc(Arena* arena, u8* ptr, isize current_size, ptrdiff_t new_size);
//
//#define arena_alloc(...)            arena_allocx(__VA_ARGS__,arena_alloc3,arena_alloc2)(__VA_ARGS__)
//#define arena_allocx(a,b,c,d,...) d
//#define arena_alloc2(a, t)          (t *)alloc(a, sizeof(t), _Alignof(t), 1, 0)
#define arena_alloc(a, t, n)       (t *)alloc(a, sizeof(t), _Alignof(t), n, 0)