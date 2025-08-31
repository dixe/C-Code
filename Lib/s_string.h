#pragma once 

#include "arena.h"
#include "custom_types.h"


typedef struct {
  u8* data;
  isize byte_len;
  isize capacity;
} s8;

#define s8_from_literal(s) (s8){s, sizeof(s)-1, sizeof(s)}

// using strlen to get byte_len and capacity.
// use this for literals, to s16 conversion
s8 s8_empty_raw(u8* buffer, isize capacity);

s8 s8_empty(Arena* arena, isize capacity);

s8 s8_from_c_str(u8* lit_str);

s8 s8_from_bytes(u8* bytes, isize count);

s8 s8_isize_to_s8(Arena* arena, isize num);

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
/// Use append to modify s1
/// </summary>
/// <param name="a"></param>
/// <param name="s1"></param>
/// <param name="s2"></param>
/// <returns></returns>
s8 s8_concat(Arena* a, s8 s1, s8 s2);


b32 s8_equals(s8 s1, s8 s2);


/// <summary>
/// Appends src[src_start;src_end] to dest. If dest has capacity, resuse memory, otherwise realloc in Arena a
/// </summary>
/// <param name="a"></param>
/// <param name="dest"></param>
/// <param name="src"></param>
/// <param name="src_start"></param>
/// <param name="src_end"></param>
void s8_append(Arena* a, s8* dest, s8 src, isize src_start, isize src_end);


/// <summary>
/// Grows the string s by additionalBytes in the give arena
/// </summary>
/// <param name="a"></param>
/// <param name="s"></param>
/// <param name="additionalBytes"></param>
/// <returns></returns>
isize s8_grow_by(Arena*, s8* s, isize additionalBytes);

/// <summary>
/// Try to parse s as i64. Parses empty string as 0
/// </summary>
/// <param name="s"></param>
/// <param name="output"></param>
/// <returns></returns>
b32 s8_try_parse_i64(s8 s, i64* output);



/// <summary>
/// Try to parse s as u32. Parses empty string as 0
/// </summary>
/// <param name="s"></param>
/// <param name="output"></param>
/// <returns></returns>
b32 s8_try_parse_u32(s8 s, u32* output);


/// <summary>
/// Try to parse s as hex into u32. Parses empty string as 0, can either start with 0x, or not
/// </summary>
/// <param name="s"></param>
/// <param name="output"></param>
/// <returns></returns>
b32 s8_try_parse_u32_hex(s8 s, u32* output);

/// <summary>
/// Try to parse s as u8. Parses empty string as 0
/// </summary>
/// <param name="s"></param>
/// <param name="output"></param>
/// <returns></returns>
b32 s8_try_parse_u8(s8 s, u8* output);

/// <summary>
/// Try to s as a f64. Parsed int before and after '.' seperatly 
/// then combining them to create the result
/// Supports 'xx' 'xx.' 'xxx.xx' '.xx'
/// </summary>
/// <param name="s"></param>
/// <param name="output"></param>
/// <returns></returns>
b32 s8_try_parse_f64(s8 s, f64* output);




typedef struct {
  c16* data;
  isize byte_len;
  isize capacity;
} s16;


#define s16_from_literal(s) (s16){s, sizeof(s)-1, sizeof(s)}

s16 s16_from_c_str(c16* lit_str);

s16 s16_empty_raw(c16* buffer, isize capacity);

s16 s16_empty(Arena* arena, isize capacity);

