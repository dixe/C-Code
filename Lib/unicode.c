#include "unicode.h"

s8 utf32_to_utf8(u32 code, Arena* a) {
  // based on table here for conversion https://en.wikipedia.org/wiki/UTF-8#Description
  s8 ret = s8_empty(a, 4);
  ret.byte_len = 0;
  if (code < 128)
  {
    ret.data[0] = (u8)code;
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



UnicodeDataArr unicode_load_data_from_file(Arena output_data, FileIter* fi)
{
  
  u8* line_data_ptr = arena_alloc(&output_data, u8, 512);
  Arena line_data = arena_create_fixed(line_data_ptr, 512);

  // maybe read whole file are get line count

  i32 lines = 0;
  s8 line = file_iter_next(&line_data, fi, '\n');;
  while (line.byte_len != 0)
  {
    lines += 1;
    line = file_iter_next(&line_data, fi, '\n');;    
    arena_reset(&line_data);
  }

  file_iter_seek(fi, 0);

  //allocate array for data
  isize start = output_data.data + output_data.offset;
  UnicodeData* data = arena_alloc(&output_data, UnicodeData, lines);  

  memset(start, 0, sizeof(UnicodeData) * lines);// output_data.offset - start);
  i32 index = 0;
  b32 run = 1;
  while (run)
  {
    s8 code = file_iter_next(&line_data, fi, ';');
    s8 name = file_iter_next(&line_data, fi, ';');
    s8 gen_cat = file_iter_next(&line_data, fi, ';');
    s8 cano_cat = file_iter_next(&line_data, fi, ';');
    s8 bidi_cat = file_iter_next(&line_data, fi, ';');
    s8 cdm = file_iter_next(&line_data, fi, ';');
    s8 deicaml = file_iter_next(&line_data, fi, ';');
    s8 digit = file_iter_next(&line_data, fi, ';');
    s8 num = file_iter_next(&line_data, fi, ';');
    s8 mirroed = file_iter_next(&line_data, fi, ';');
    s8 v1_name = file_iter_next(&line_data, fi, ';');
    s8 comment = file_iter_next(&line_data, fi, ';');
    s8 upper = file_iter_next(&line_data, fi, ';');
    s8 lower = file_iter_next(&line_data, fi, ';');
    s8 title = file_iter_next(&line_data, fi, '\n');

    run = code.byte_len > 0;

    b32 parsed = s8_try_parse_u32_hex(code, &data[index].code);

   if (!parsed)
    {
      exit(231);
    }
    data[index].char_name = s8_empty(&output_data, 0);
    s8_append(&output_data, &data[index].char_name, name, 0, name.byte_len);
    

    parsed = s8_try_parse_u8(code, &data[index].uppercase_mapping);
    parsed = s8_try_parse_u8(code, &data[index].lowercase_mapping);
    parsed = s8_try_parse_u8(code, &data[index].titlcase_mapping);   

    arena_reset(&line_data);
    index += 1;

  }

  UnicodeDataArr ret = { 0 };
  ret.data = data;
  ret.len = lines;
  return ret;

  



}