#pragma once
#include <Windows.h>

typedef struct {
  int size;
  int offset;
  char* data;
} Arena;

Arena* create_arena(int bytes);


void* arena_alloc(Arena* arena, int  bytes);

void arena_reset(Arena* arena);