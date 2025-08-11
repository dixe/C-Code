#pragma once 

#include "arena.h"
#include "custom_types.h"


typedef struct {
  c16* data;
  size len;
  size capacity;
} s_string;


s_string s_from_literal(c16* lit_str);

s_string s_empty_raw(c16* buffer, size capacity);

s_string s_empty(Arena* arena, size capacity);