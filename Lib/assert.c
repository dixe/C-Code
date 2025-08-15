#pragma once
#include "custom_types.h"
#include <stdio.h>

void a_assert(b32 expression, char* expr_str, char* file, i32 line)
{
  if (expression)
  {
    return;
  }
  printf("Expr: '%s' was not true, on line: '%d' in file: '%s'", expr_str, line, file);
  exit(-1);
}

