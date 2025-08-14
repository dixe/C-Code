#pragma once 

#include "arena.h"
#include "custom_types.h"


typedef struct {
  c16* data;
  isize len;
  isize capacity;
} s16;


s16 s16_from_literal(c16* lit_str);

s16 s16_empty_raw(c16* buffer, isize capacity);

s16 s16_empty(Arena* arena, isize capacity);

typedef struct {
  u8* data;
  isize len;
  isize capacity;
} s8;


s8 s8_from_literal(u8* lit_str);

s8 s8_empty_raw(u8* buffer, isize capacity);

s8 s8_empty(Arena* arena, isize capacity);

void s8_print(s8 s);

void s8_println(s8 s);

/// <summary>
/// Find char (chr) in s -1 on not found
/// </summary>
/// <param name="s"></param>
/// <param name="chr"></param>
/// <returns></returns>
isize s8_find_char(isize index, s8 s, u8 chr);

/// <summary>
/// Substring with start and end pos allocate the data for return string in the arena a
/// </summary>
/// <param name="a"></param>
/// <param name="s"></param>
/// <param name="start"></param>
/// <param name="end"></param>
/// <returns></returns>
s8 s8_substring(Arena* a, s8 s, isize start, isize end);


/// <summary>
/// Concat s1 and s2 into a new string allocated in arena
/// s1 and s2 are not touched
/// </summary>
/// <param name="a"></param>
/// <param name="s1"></param>
/// <param name="s2"></param>
/// <returns></returns>
s8 s8_concat(Arena* a, s8 s1, s8 s2);



void s8_append(Arena* a, s8* dest, s8 src, isize src_start, isize src_end);


/// <summary>
/// Grows the string s by additionalBytes in the give arena
/// </summary>
/// <param name="a"></param>
/// <param name="s"></param>
/// <param name="additionalBytes"></param>
/// <returns></returns>
isize s8_grow_by(Arena*, s8* s, isize additionalBytes);