#pragma once 
#include <windows.h>

typedef struct {
  wchar_t* data;
  int len;
  int capacity;
} s_string;