#include "s_string.h"
#include "arena.h"

s16 s16_from_c_str(c16* lit_str) {
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


s8 s8_from_c_str(u8* lit_str) {  
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

void s8_println(s8 s)
{
  s8_print(s);
  s8_print(s8_from_literal("\n"));
}


u64 s8_print(s8 s) 
{
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  u64 written = 0;
  int succ = WriteFile(hOut, s.data, s.len, &written, 0);

  return written;

}


isize s8_find_char(isize start_index, s8 s, u8 chr) {
  for (isize i = start_index; i < s.len; i++)
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
  if (len <= 0 || start >= s.len  || end > s.len)
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


s8 s8_concat(Arena* a, s8 s1, s8 s2)
{
  isize len = s1.len + s2.len;
  s8 ret = s8_empty(a, len);

  memcpy(ret.data, s1.data, s1.len);
  memcpy(ret.data + s1.len, s2.data, s2.len);

  ret.len = len;
  return ret;
}



void s8_append(Arena* a, s8* dest, s8 src, isize src_start, isize src_end)
{
  // check if we have capacity, otherwise copy grow dest
  isize space = dest->capacity - dest->len;
  isize src_len = src_end - src_start;
  if (space < src_end - src_start)
  {

    isize additional = src_end - src_start - space;
    isize err = s8_grow_by(a, dest, additional);
  }
  memcpy(dest->data + dest->len, src.data + src_start, src_len);
  dest->len += src_len;
}

isize s8_grow_by(Arena* a, s8* s, isize additionalBytes)
{
  // fix this so we copy if new alloc
  u8* new_ptr = (u8*)arena_realloc(a, s->data, s->capacity, s->capacity + additionalBytes );
  s->capacity = s->capacity + additionalBytes;

  if (new_ptr == s->data)
  {
    return 0;
  }
  
  // copy old data
  memcpy(new_ptr, s->data, s->len);
  s->data = new_ptr; 
  
  return 0;
}


b32 s8_try_parse_int(s8 s, i64* output)
{
  // TODO skip whitespace
  *output = 0;
  for (isize i = 0; i < s.len; i++)
  {
    *output *= 10;
    if (s.data[i] < 48 || s.data[i] >57)
    {
      return 0;
    }
    *output += s.data[i] - 48;
  }
  return 1;
}


b32 s8_try_parse_f64(s8 s, f64* output)
{
  isize dot_pos = s8_find_char(0, s, '.');
  b32 parsed;
  i64 as_int;
  if (dot_pos == -1 || dot_pos + 1 == s.len)
  {
    // no dot or last dot
    if (dot_pos + 1 == s.len)
    {
      s.len -= 1; // s passed by value, so changes are local
    }
    parsed = s8_try_parse_int(s, &as_int);
    *output = (f64)as_int;
    return parsed;
  }

  // dot found
  s8 int_part_s = { 0 };
  int_part_s.data = s.data;
  int_part_s.len = dot_pos; 
  parsed = s8_try_parse_int(int_part_s, &as_int);
  if (parsed != 1)
  {
    return 1;
  }

  s8 frac_part_s = { 0 };
  i64 as_int2;
  frac_part_s.data = s.data + dot_pos + 1;
  frac_part_s.len = s.len - dot_pos -1;
  parsed = s8_try_parse_int(frac_part_s, &as_int2);
  
  if (parsed != 1)
  {
    return 1;
  }
  
  f64 div = 1;
  // pow
  for (isize i = 0; i < frac_part_s.len; i++)
  {
    div *= 10;
  }
  *output = as_int2 * 1.0 / div;
  *output += (f64)as_int;

  return parsed;

}
