#include "s_string.h"
#include "arena.h"

s16 s16_from_c_str(c16* lit_str) {
  s16 s;
  s.data = lit_str;
  s.byte_len = wcslen(lit_str);
  s.capacity = s.byte_len;
  return s;
}

s16 s16_empty_raw(c16* buffer, isize capacity)
{
  s16 s;
  s.data = buffer;
  s.byte_len = 0;
  s.capacity = capacity;
  return s;
}

s16 s16_empty(Arena* arena, isize capacity)
{
  c16* buffer = arena_alloc(arena, c16, capacity);
  
  s16 s;
  s.data = buffer;
  s.byte_len = 0;
  s.capacity = capacity;
  return s;
}


s8 s8_from_c_str(u8* lit_str) {  
  s8 s;
  s.data = lit_str;
  s.byte_len = strlen(lit_str);
  s.capacity = s.byte_len;
  return s;
}

s8 s8_from_bytes(u8* bytes, isize count) {
  s8 s;
  s.data = bytes;
  s.byte_len = count;
  s.capacity = s.byte_len;
  return s;
}

s8 s8_empty_raw(u8* buffer, isize capacity)
{
  s8 s;
  s.data = buffer;
  s.byte_len = 0;
  s.capacity = capacity;
  return s;
}

s8 s8_empty(Arena* arena, isize capacity)
{
  s8 s;
  s.data = 0;
  s.byte_len = 0;
  s.capacity = capacity;
  if (capacity == 0)
  {
    return s;
  }

  u8* buffer = arena_alloc(arena, u8, capacity);
  s.data = buffer;
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
  int succ = WriteFile(hOut, s.data, (DWORD)s.byte_len, (LPDWORD)&written, 0);

  return written;
}

isize s8_find_char(isize start_index, s8 s, u8 chr) {
  for (isize i = start_index; i < s.byte_len; i++)
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

  isize byte_len = end - start ;
  
  // check out of ranges
  if (byte_len <= 0 || start >= s.byte_len  || end > s.byte_len)
  {
    return ret;
  }

  ret = s8_empty(a, end - start);

  if (ret.data == 0)
  {
    return ret;
  }

  memcpy(ret.data, s.data + start, byte_len);

  ret.byte_len = byte_len;

  return ret;
}

s8 s8_concat(Arena* a, s8 s1, s8 s2)
{
  isize byte_len = s1.byte_len + s2.byte_len;
  s8 ret = s8_empty(a, byte_len);

  memcpy(ret.data, s1.data, s1.byte_len);
  memcpy(ret.data + s1.byte_len, s2.data, s2.byte_len);

  ret.byte_len = byte_len;
  return ret;
}

b32 s8_equals(s8 s1, s8 s2)
{
  if (s1.byte_len != s2.byte_len)
  {
    return 0;
  }
  i32 cmp = memcmp(s1.data, s2.data, s1.byte_len);
  return cmp == 0;
}

void s8_append(Arena* a, s8* dest, s8 src, isize src_start, isize src_end)
{
  // check if we have capacity, otherwise copy grow dest
  isize space = dest->capacity - dest->byte_len;
  isize src_len = src_end - src_start;
  if (space < src_end - src_start)
  {

    isize additional = src_end - src_start - space;
    isize err = s8_grow_by(a, dest, additional);
  }
  memcpy(dest->data + dest->byte_len, src.data + src_start, src_len);

  dest->byte_len += src_len;
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
  memcpy(new_ptr, s->data, s->byte_len);
  s->data = new_ptr; 
  
  return 0;
}

b32 s8_try_parse_i64(s8 s, i64* output)
{
  // TODO skip whitespace
  // working with negative numbers
  *output = 0;
  for (isize i = 0; i < s.byte_len; i++)
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

b32 s8_try_parse_u8(s8 s, u8* output)
{
  u32 d = 0;
  b32 u32_res = s8_try_parse_u32(s, &d);
  if(!u32_res || d > 0xFF)
  {
    return 0;
  }
  output = (u8)d;
  return 1; 
}

b32 s8_try_parse_u32_hex(s8 s, u32* output)
{
  // TODO skip whitespace  
  *output = 0;
  for (isize i = 0; i < s.byte_len; i++)
  {
    *output *= 16;

    // 0-9
    if (s.data[i] > 47 && s.data[i] < 58)
    {
      *output += s.data[i] - 48;
      continue;
    }
    // A-F
    if (s.data[i] > 64 && s.data[i] < 71)
    {
      *output += s.data[i] - 55;
      continue;
    }

    // a-f
    if (s.data[i] > 96 && s.data[i] < 103)
    {
      *output += s.data[i] - 87;
      continue;
    }

    // X or x
    if (s.data[i] == 88 || s.data[i] == 120)
    {
      *output = 0;
      continue;
    }
    return 0;
  }
  return 1;
}

b32 s8_try_parse_u32(s8 s, u32* output)
{
  // TODO skip whitespace
  *output = 0;
  for (isize i = 0; i < s.byte_len; i++)
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
  b32 parsed = 0;
  i64 as_int = 0;
  if (dot_pos == -1 || dot_pos + 1 == s.byte_len)
  {
    // no dot or last dot
    if (dot_pos + 1 == s.byte_len)
    {
      s.byte_len -= 1; // s passed by value, so changes are local
    }
    parsed = s8_try_parse_i64(s, &as_int);
    *output = (f64)as_int;
    return parsed;
  }

  // dot found
  s8 int_part_s = { 0 };
  int_part_s.data = s.data;
  int_part_s.byte_len = dot_pos; 
  parsed = s8_try_parse_i64(int_part_s, &as_int);
  if (parsed != 1)
  {
    return 1;
  }

  s8 frac_part_s = { 0 };
  i64 as_int2 = 0;;
  frac_part_s.data = s.data + dot_pos + 1;
  frac_part_s.byte_len = s.byte_len - dot_pos -1;
  parsed = s8_try_parse_i64(frac_part_s, &as_int2);
  
  if (parsed != 1)
  {
    return 1;
  }
  
  f64 div = 1;
  // pow
  for (isize i = 0; i < frac_part_s.byte_len; i++)
  {
    div *= 10;
  }
  *output = as_int2 * 1.0 / div;
  *output += (f64)as_int;

  return parsed;

}
