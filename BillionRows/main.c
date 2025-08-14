#include <stdio.h>
#include "arena.h"
#include "s_string.h"
#include "custom_types.h"
#include "arena.h"


typedef struct {
  s8 parsed_s;
  isize index;
} LoadMore;

LoadMore load_more(Arena* line_arena, s8* input_string, isize index, FILE* fptr, isize chunk_size, u8 chr);



typedef struct {
  s8 input_string;
  FILE* fptr;  
  Arena* line_arena;
  isize index;

} FileIter;

s8 iter_next(FileIter* iter, u8 chr) {
  // parse data until chr
  // split on ; and get array of #; + 1
  
  arena_reset(iter->line_arena);

  isize chr_pos = s8_find_char(iter->index, iter->input_string, chr);

  // either we did not load enough, or file is read to end;
  // should just alloca full arena cap as empty string1
  s8 ret = s8_empty(iter->line_arena, iter->line_arena->cap);
  if (iter->input_string.len == 0)
  {
    iter->input_string.len = fread(iter->input_string.data, sizeof(u8), iter->input_string.capacity, iter->fptr);
    iter->index = 0;
    if (iter->input_string.len == 0)
    {
      return ret;
    }
  }
  while (chr_pos == -1)
  {
    // append substring of inputstring to ret
    s8_append(iter->line_arena, &ret, iter->input_string, iter->index, iter->input_string.len);

    iter->input_string.len = fread(iter->input_string.data, sizeof(u8), iter->input_string.capacity, iter->fptr);
    iter->index = 0;
    if (iter->input_string.len == 0)
    {
      return ret;
    }
    
    chr_pos = s8_find_char(iter->index, iter->input_string, chr);
  }

  // copy 

  s8_append(iter->line_arena, &ret, iter->input_string, iter->index, chr_pos);
  iter->index = chr_pos + 1;
  return ret;
}

// could just be and empty input str
isize init_iter(FileIter* iter, char * str, s8 buffer,  Arena* line_arena) {

  i32 err = fopen_s(&iter->fptr, str, "r");
  // remaining space for input string
  iter->input_string = buffer;
  iter->index = 0;
  iter->line_arena = line_arena;
  return 0;
}

int main()
{
  isize chunk_size = 256;
  Arena* input_data = arena_create(chunk_size);

  FileIter* iter = arena_alloc(input_data, FileIter);

  s8 buffer = s8_empty(input_data, input_data->cap - input_data->offset);

  isize line_size = 256;
  Arena* line_arena = arena_create(line_size);
  

  init_iter(iter, "E:\\repos\\C-Code\\BillionRows\\meassurement.txt", buffer, line_arena);  

  s8 line = iter_next(iter, '\n');
  while (line.len > 0) {
    // use whole line
    s8_println(line);
    line = iter_next(iter, '\n');
  }

  return 0;
  FILE* fptr;

  // 1 mb chuncks for data
  s8 input_string = s8_empty(input_data, chunk_size);

  
  // Open a file in read mode
  i32 err = fopen_s(&fptr, "E:\\repos\\C-Code\\BillionRows\\meassurement.txt", "r");

  input_string.len = fread(input_string.data, sizeof(u8), chunk_size, fptr);



  isize index = 0;

  // maybe make this into a line reader?
  // store state in a struct and have a next function return the line?
  // parse lines, loading new from file as we go
  while (index < input_string.len )
  {
    // parse data until ;
    // split on ; and get array of #; + 1

    isize semicolon_pos = s8_find_char(index, input_string, ';');
    s8 name = { 0 };
    if (semicolon_pos == -1)
    {      
      LoadMore load_data = load_more( line_arena, &input_string, index, fptr, chunk_size, ';');
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
      LoadMore load_data = load_more(line_arena, &input_string, index, fptr, chunk_size, '\n');
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



    // do stuff with line name and temp

    // lookup name and upate min, max avg
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
LoadMore load_more(Arena* line_arena, s8* input_string, isize index, FILE* fptr, isize chunk_size, u8 chr)
{

  s8 start = s8_substring(line_arena, *input_string, index, input_string->len);

  //  just overwrite input_string data?, we set the len, so maybe ok?
  input_string->len = fread(input_string->data, sizeof(u8), chunk_size, fptr);
  index = 0;
  isize char_pos = s8_find_char(index, *input_string, chr);

  s8 end = s8_substring(line_arena, *input_string, index, char_pos);

  LoadMore ret = { 0 };
  ret.index = char_pos;
  ret.parsed_s = s8_concat(line_arena, start, end);
  return ret;
}