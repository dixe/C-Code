#pragma once
#include <Windows.h>
#include "custom_types.h"

typedef struct {
  size size;
  size offset;
  byte* data;
} Arena;

Arena* create_arena(size bytes);

void* arena_alloc(Arena* arena, size bytes);

void arena_reset(Arena* arena);