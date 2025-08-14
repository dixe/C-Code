#include "custom_types.h"
#include "s_string.h"
#include <stdio.h>

typedef struct {
  s8 input_string;
  FILE* fptr;
  Arena* line_arena;
  isize index;
} FileIter;

s8 file_iter_next(FileIter* iter, u8 chr) {
  arena_reset(iter->line_arena);
  s8 ret = s8_empty(iter->line_arena, iter->line_arena->cap);

  if (iter->fptr == 0)
  {
    return ret;
  }

  isize chr_pos = s8_find_char(iter->index, iter->input_string, chr);

  // either we did not load enough, or file is read to end;
  // should just alloca full arena cap as empty string1
  
  if (iter->input_string.len == 0)
  {
    iter->input_string.len = fread(iter->input_string.data, sizeof(u8), iter->input_string.capacity, iter->fptr);
    iter->index = 0;
    if (iter->input_string.len == 0)
    {
      fclose(iter->fptr);
      iter->fptr = 0;
      return ret;
    }
    chr_pos = s8_find_char(iter->index, iter->input_string, chr);
  }
  while (chr_pos == -1)
  {
    // append substring of inputstring to ret
    s8_append(iter->line_arena, &ret, iter->input_string, iter->index, iter->input_string.len);

    iter->input_string.len = fread(iter->input_string.data, sizeof(u8), iter->input_string.capacity, iter->fptr);
    iter->index = 0;
    if (iter->input_string.len == 0)
    {
      fclose(iter->fptr);
      iter->fptr = 0;
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
isize file_init_iter(FileIter* iter, char* str, s8 buffer, Arena* line_arena) {

  i32 err = fopen_s(&iter->fptr, str, "r");
  // remaining space for input string
  iter->input_string = buffer;
  iter->index = 0;
  iter->line_arena = line_arena;
  return 0;
}



void file_iter_close(FileIter* iter)
{
  if (iter->fptr != 0)
  {
    fclose(iter->fptr);
    iter->fptr = 0;
  }
}