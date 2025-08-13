#include <stdio.h>
#include "arena.h"
#include "s_string.h"
#include "custom_types.h"
#include "arena.h"


typedef struct {
  s8 parsed_s;
  isize index;
} LoadMore;

LoadMore load_more(Arena* line_arena, s8 input_string, isize index, FILE* fptr, isize chunk_size, u8 chr);


int main()
{
  FILE* fptr;

  // 1 mb chuncks for data
  isize chunk_size = 128;
  Arena* input_data = arena_create(chunk_size);

  s8 input_string = s8_empty(input_data, chunk_size);

  
  // Open a file in read mode
  i32 err = fopen_s(&fptr, "E:\\repos\\C-Code\\BillionRows\\meassurement.txt", "r");

  input_string.len = fread(input_string.data, sizeof(u8), chunk_size, fptr);


  // assume 32 is enough for each line
  isize line_size = 32;
  Arena* line_arena = arena_create(line_size);

  isize index = 0;

  // parse lines, loading new from file as we go
  while (index < input_string.len )
  {
    // parse data until ;
    // split on ; and get array of #; + 1

    isize semicolon_pos = s8_find_char(index, input_string, ';');
    s8 name = { 0 };
    if (semicolon_pos == -1)
    {      
      LoadMore load_data = load_more( line_arena, input_string, index, fptr, chunk_size, ';');
      name = load_data.parsed_s;
      index = load_data.index + 1;
    }
    else {
      name = s8_substring(line_arena, input_string, index, semicolon_pos);
      index = semicolon_pos + 1;
    }

    isize line_end_pos = s8_find_char(index, input_string, '\n');
    s8 temp = { 0 };
    if (line_end_pos == -1)
    {
      LoadMore load_data = load_more(line_arena, input_string, index, fptr, chunk_size, '\n');
      temp = load_data.parsed_s;
      index = load_data.index + 1;
      if (load_data.index == -1)
      {
        // we are done
        arena_reset(line_arena);
        return 0;
      }
    }
    else {
      temp = s8_substring(line_arena, input_string, index , line_end_pos);
      index = line_end_pos + 1;
    }

    s8_print(name);
    s8_print(s8_from_literal(" "));
    s8_println(temp);

    arena_reset(line_arena);
  }


}


// either found or end of read data
// load more data into input_string,
// otherwise we would have split on it
// store in line arena
LoadMore load_more(Arena* line_arena, s8 input_string, isize index, FILE* fptr, isize chunk_size, u8 chr)
{

  s8 start = s8_substring(line_arena, input_string, index, input_string.len);

  //  just overwrite input_string data?, we set the len, so maybe ok?
  input_string.len = fread(input_string.data, sizeof(u8), chunk_size, fptr);
  index = 0;
  isize char_pos = s8_find_char(index, input_string, chr);

  s8 end = s8_substring(line_arena, input_string, index, char_pos);

  LoadMore ret = { 0 };
  ret.index = char_pos;
  ret.parsed_s = s8_concat(line_arena, start, end);
  return ret;
}