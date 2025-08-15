#pragma once

#include <Windows.h>
#include "custom_types.h"

typedef struct {
  isize cap;
  isize offset;
  u8* data;
} Arena;



Arena arena_create(isize bytes);

void* alloc(Arena* a, ptrdiff_t isize, ptrdiff_t align, ptrdiff_t count, i32 flags);

void arena_reset(Arena* arena);

/// <summary>
/// Try to reallocate a pointer with current_size, to a new pointer with new_size
/// If ptr was last pointer allocated, the allocation are extended, otherwise reallocated in arena, old pointer still valid. Not release.
/// </summary>
/// <param name="arena"></param>
/// <param name="ptr"></param>
/// <param name="current_size"></param>
/// <param name="new_size"></param>
/// <returns></returns>
void* arena_realloc(Arena* arena, u8* ptr, isize current_size, ptrdiff_t new_size);

#define arena_alloc(...)            arena_allocx(__VA_ARGS__,arena_alloc3,arena_alloc2)(__VA_ARGS__)
#define arena_allocx(a,b,c,d,...) d
#define arena_alloc2(a, t)          (t *)alloc(a, sizeof(t), _Alignof(t), 1, 0)
#define arena_alloc3(a, t, n)       (t *)alloc(a, sizeof(t), _Alignof(t), n, 0)