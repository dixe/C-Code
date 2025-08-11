#pragma once 

#include "arena.h"
#include "custom_types.h"


typedef struct {
  c16* data;
  xsize len;
  xsize capacity;
} s16;


s16 s_from_literal(c16* lit_str);

s16 s_empty_raw(c16* buffer, xsize capacity);

s16 s_empty(Arena* arena, xsize capacity);