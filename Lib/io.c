#include "io.h"
#include <Windows.h>



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
