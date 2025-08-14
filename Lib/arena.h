#pragma once

#include <Windows.h>
#include "custom_types.h"

typedef struct {
  isize cap;
  isize offset;
  u8* data;
} Arena;



Arena* arena_create(isize bytes);

void* alloc(Arena* a, ptrdiff_t isize, ptrdiff_t align, ptrdiff_t count, i32 flags);

void arena_reset(Arena* arena);

void* arena_realloc(Arena* arena, u8* ptr, isize current_size, ptrdiff_t new_size);

#define arena_alloc(...)            arena_allocx(__VA_ARGS__,arena_alloc3,arena_alloc2)(__VA_ARGS__)
#define arena_allocx(a,b,c,d,...) d
#define arena_alloc2(a, t)          (t *)alloc(a, sizeof(t), _Alignof(t), 1, 0)
#define arena_alloc3(a, t, n)       (t *)alloc(a, sizeof(t), _Alignof(t), n, 0)