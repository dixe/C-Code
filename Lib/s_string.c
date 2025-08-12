#include "s_string.h"
#include <stdio.h>
#include "arena.h"

// using strlen to get len and capacity.
// use this for literals, to s16 conversion
s16 s16_from_literal(c16* lit_str) {
  s16 s;
  s.data = lit_str;
  s.len = wcslen(lit_str);
  s.capacity = s.len;
  return s;
}

s16 s16_empty_raw(c16* buffer, isize capacity)
{
  s16 s;
  s.data = buffer;
  s.len = 0;
  s.capacity = capacity;
  return s;
}

s16 s16_empty(Arena* arena, isize capacity)
{
  c16* buffer = arena_alloc(arena, c16, capacity);
  
  s16 s;
  s.data = buffer;
  s.len = 0;
  s.capacity = capacity;
  return s;
}



// using strlen to get len and capacity.
// use this for literals, to s16 conversion
s8 s8_from_literal(u8* lit_str) {
  s8 s;
  s.data = lit_str;
  s.len = strlen(lit_str);
  s.capacity = s.len;
  return s;
}

s8 s8_empty_raw(u8* buffer, isize capacity)
{
  s8 s;
  s.data = buffer;
  s.len = 0;
  s.capacity = capacity;
  return s;
}

s8 s8_empty(Arena* arena, isize capacity)
{
  u8* buffer = arena_alloc(arena, u8, capacity);

  s8 s;
  s.data = buffer;
  s.len = 0;
  s.capacity = capacity;
  return s;
}

void s8_print(s8 s) 
{

  for (int i = 0; i < s.len; i++)
  {
    putchar(s.data[i]);
  }

}


isize s8_find_char(s8 s, u8 chr) {
  for (isize i = 0; i < s.len; i++)
  {
    if (chr == s.data[i])
    {
      return i;
    }
  }
  return -1;
}

s8 s8_substring(Arena* a, s8 s, isize start, isize end) {
  s8 ret = { 0 };

  isize len = end - start ;
  
  // check out of ranges
  if (len <= 0 || start >= s.len  || end >= s.len)
  {
    return ret;
  }

  ret = s8_empty(a, end - start);

  if (ret.data == 0)
  {
    return ret;
  }

  memcpy(ret.data, s.data + start, len);

  ret.len = len;

  return ret;

}