#include <stdio.h>
#include "arena.h"
#include "s_string.h"
#include "custom_types.h"
#include "arena.h"


int main()
{

  FILE* fptr;

  // 1 mb chuncks for data
  isize size = 16;
  Arena* input_data = arena_create(size);

  s8 input_string = s8_empty(input_data, size);

  
  // Open a file in read mode
  i32 err = fopen_s(&fptr, "E:\\repos\\C-Code\\BillionRows\\meassurement.txt", "r");

  input_string.len = fread(input_string.data, sizeof(u8), size, fptr);


  // assume 32 is enough for each line
  isize line_size = 32;
  Arena* line_arena = arena_create(line_size);

  // parse lines, loading new from file as we go
  while (input_string.len != 0 || line ended)
  {


    // parse data until ;
    // split on ; and get array of #; + 1
    // 

    isize semicolon_pos = s8_find_char(input_string, ';');


    isize line_end_pos = s8_find_char(input_string, '\n');

    s8 name = s8_substring(line_arena, input_string, 0, semicolon_pos);
    s8 temp = s8_substring(line_arena, input_string, semicolon_pos + 1, line_end_pos);


    s8_print(name);
    s8_print(s8_from_literal(" "));
    s8_println(temp);


    input_string.len = fread(input_string.data, sizeof(u8), size, fptr);

    arena_reset(line_arena);
  }


  //input_string.len = real_size;

  
  // read file in chuncks

  // parse each row

  // use hashset to store data of min, max sum and count

  // loop over hashset and get abg, min and max

}