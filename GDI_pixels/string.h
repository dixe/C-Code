#pragma once 
#include <windows.h>
#include "arena.h"


typedef struct {
  wchar_t* data;
  size_t len;
  size_t capacity;
} s_string;


s_string s_from_literal(wchar_t* lit_str);

s_string s_empty_raw(wchar_t* buffer, int capacity);


s_string s_empty(Arena* arena, int capacity);