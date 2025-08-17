#include "unicode.h"

s8 utf32_to_utf8(u32 code, Arena* a) {
  // based on table here for conversion https://en.wikipedia.org/wiki/UTF-8#Description
  s8 ret = s8_empty(a, 4);
  ret.byte_len = 0;
  if (code < 128)
  {
    ret.data[0] = code;
    ret.byte_len = 1;
  }
  else if (code < 0x800) // xxxyyyyzzzz
  {
    ret.data[0] = 0b11000000 | (code >> 6);
    ret.data[1] = 0b10000000 | (code & 0x3F);
    ret.byte_len = 2;
  }
  else if (code < 0x8000) // wwwwxxxxyyyyzzzz	
  {
    ret.data[0] = 0b11100000 | (code >> 12);
    ret.data[1] = 0b10000000 | ((code >> 6) & 0x3F);
    ret.data[2] = 0b10000000 | (code & 0x3F);
    ret.byte_len = 3;
  }
  else
  {
    ret.data[0] = 0b11110000 | (code >> 18);
    ret.data[1] = 0b10000000 | ((code >> 12) & 0x3F);
    ret.data[2] = 0b10000000 | ((code >> 6) & 0x3F);
    ret.data[3] = 0b10000000 | (code & 0x3F);
    ret.byte_len = 4;
  }
  return ret;
}
