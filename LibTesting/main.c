#include <stdio.h>
#include "arena.h"
#include "s_string.h"
#include "custom_types.h"
#include "arena.h"
#include "file.h"
#include "assert.h"
#include <windows.h>

void parse_f64_tests();

int main()
{
  parse_f64_tests();

  printf("All passed");
}

void parse_f64_tests() {

  // par
  i64 parsed = 0;

  s8_try_parse_int(s8_from_literal("123"), &parsed);
  ASSERT(parsed == 123);
  f64 parsed_f;

  s8_try_parse_f64(s8_from_literal("32"), &parsed_f);
  ASSERT(parsed_f == 32.0);
  s8_try_parse_f64(s8_from_literal("12."), &parsed_f);
  ASSERT(parsed_f == 12.0);
  s8_try_parse_f64(s8_from_literal(".12"), &parsed_f);
  ASSERT(parsed_f == .12);
  s8_try_parse_f64(s8_from_literal("123.32"), &parsed_f);
  ASSERT(parsed_f == 123.32);

}