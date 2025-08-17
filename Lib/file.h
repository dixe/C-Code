#pragma once
#include "s_string.h"


typedef struct {
  s8 input_string;
  FILE* fptr;
  isize index;
} FileIter;

/// <summary>
/// Return the next chunk of string, stopping when chr is reached
/// </summary>
/// <param name="iter"></param>
/// <param name="chr"></param>
/// <returns></returns>
s8 file_iter_next(Arena* a, FileIter* iter, u8 chr);

/// <summary>
/// Initialize a fileIter with the given path
/// </summary>
/// <param name="iter">FileIter</param>
/// <param name="path">path to file</param>
/// <param name="buffer">Buffer for hold chunkcs of the file </param>
/// <param name="line_arena">Arena for allocatio the lines returned by iter_next</param>
/// <returns></returns>
isize file_init_iter(FileIter* iter, char* path, s8 buffer);



void file_iter_close(FileIter* iter);