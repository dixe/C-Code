#include "string.h"
#include <stdio.h>
#include "arena.h"
// using strlen to get len and capacity.
// use this for literals, to s_string conversion


s_string s_from_literal(c16* lit_str) {
  s_string s;
  s.data = lit_str;
  s.len = wcslen(lit_str);
  s.capacity = s.len;
  return s;
}

s_string s_empty_raw(c16* buffer, size capacity)
{
  s_string s;
  s.data = buffer;
  s.len = 0;
  s.capacity = capacity;
  return s;
}

s_string s_empty(Arena* arena, size capacity)
{
  c16* buffer = arena_alloc(arena, capacity);
  
  s_string s;
  s.data = buffer;
  s.len = 0;
  s.capacity = capacity;
  return s;
}