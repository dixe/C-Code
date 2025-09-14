#include "custom_types.h"
#include "s_string.h"
#include <stdio.h>
#include "file.h"




s8 file_read_all(Arena *a, FILE* fptr)
{

  fseek(fptr, 0L, SEEK_END);

  isize file_size = ftell(fptr);
  fseek(fptr, 0L, SEEK_SET);

  s8 ret = s8_empty(a, file_size);

  ret.byte_len = fread(ret.data, sizeof(u8), ret.capacity, fptr);

  return ret;

}

s8 file_iter_next(Arena* a, FileIter* iter, u8 chr) {
  // Mabe allocate 8/16/32/64 bytes, depending on the arena cap
  s8 ret = { 0 };

  if (iter->fptr == 0)
  {
    return ret;
  }

  isize chr_pos = s8_find_char(iter->index, iter->input_string, chr);

  // either we did not load enough, or file is read to end;
  // should just alloca full arena cap as empty string1
  
  if (iter->input_string.byte_len == 0)
  {
    iter->input_string.byte_len = fread(iter->input_string.data, sizeof(u8), iter->input_string.capacity, iter->fptr);
    iter->index = 0;
    if (iter->input_string.byte_len == 0)
    {
      return ret;
    }
    chr_pos = s8_find_char(iter->index, iter->input_string, chr);
  }
  while (chr_pos == -1)
  {
    // append substring of inputstring to ret
    s8_append_substring(a, &ret, iter->input_string, iter->index, iter->input_string.byte_len);

    iter->input_string.byte_len = fread(iter->input_string.data, sizeof(u8), iter->input_string.capacity, iter->fptr);
    iter->index = 0;
    if (iter->input_string.byte_len == 0)
    {
      return ret;
    }

    chr_pos = s8_find_char(iter->index, iter->input_string, chr);
  }

  // copy 
  s8_append_substring(a, &ret, iter->input_string, iter->index, chr_pos);
  iter->index = chr_pos + 1;
  return ret;
}

// could just be and empty input str
isize file_init_iter(FileIter* iter, char* str, s8 buffer) {

  i32 err = fopen_s(&iter->fptr, str, "r");
  // remaining space for input string
  iter->input_string = buffer;
  iter->index = 0;
  return 0;
}

void file_iter_seek(FileIter* iter, isize location)
{
  fseek(iter->fptr, location, 0);
}

void file_iter_close(FileIter* iter)
{
  if (iter->fptr != 0)
  {
    fclose(iter->fptr);
    iter->fptr = 0;
  }
}