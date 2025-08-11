#include "s_string.h"
#include <stdio.h>
#include "arena.h"

// using strlen to get len and capacity.
// use this for literals, to s16 conversion
s16 s_from_literal(c16* lit_str) {
  s16 s;
  s.data = lit_str;
  s.len = wcslen(lit_str);
  s.capacity = s.len;
  return s;
}

s16 s_empty_raw(c16* buffer, xsize capacity)
{
  s16 s;
  s.data = buffer;
  s.len = 0;
  s.capacity = capacity;
  return s;
}

s16 s_empty(Arena* arena, xsize capacity)
{
  c16* buffer = arena_alloc(arena, c16, 128);
  
  s16 s;
  s.data = buffer;
  s.len = 0;
  s.capacity = capacity;
  return s;
}