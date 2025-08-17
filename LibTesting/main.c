#include <stdio.h>
#include "arena.h"
#include "s_string.h"
#include "custom_types.h"
#include "arena.h"
#include "file.h"
#include "assert.h"
#include <windows.h>
#include <stdio.h>
#include "unicode.h"

void parse_f64_tests();
void u32_to_utf8_tests();


int main()
{
  parse_f64_tests();
  u32_to_utf8_tests();

  printf("All passed");
}

void u32_to_utf8_tests()
{
  Arena a = arena_create(128);

  s8 s_a = utf32_to_utf8(65, &a);
  ASSERT(s_a.data[0] == 0x41);

  s_a = utf32_to_utf8(198, &a);
  ASSERT(s_a.data[0] == 0xc3);
  ASSERT(s_a.data[1] == 0x86);

  s_a = utf32_to_utf8(0x00001F04, &a);
  ASSERT(s_a.data[0] == 0xe1);
  ASSERT(s_a.data[1] == 0xbc);
  ASSERT(s_a.data[2] == 0x84);
  s_a = utf32_to_utf8(0x1F47B, &a);
  ASSERT(s_a.data[0] == 0xf0);
  ASSERT(s_a.data[1] == 0x9f);
  ASSERT(s_a.data[2] == 0x91);
  ASSERT(s_a.data[3] == 0xbb);
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