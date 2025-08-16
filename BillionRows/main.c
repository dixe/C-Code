#include <stdio.h>
#include "arena.h"
#include "s_string.h"
#include "custom_types.h"
#include "arena.h"
#include "file.h"
#include "hashmap.h"

int main()
{

  isize chunk_size = 256;
  Arena input_data = arena_create(chunk_size);

  FileIter* iter = arena_alloc(&input_data, FileIter);

  s8 buffer = s8_empty(&input_data, input_data.cap - input_data.offset);

  isize line_size = 256;
  Arena line_arena = arena_create(line_size);
  
  Arena perm = arena_create(32);

  file_init_iter(iter, "E:\\repos\\C-Code\\BillionRows\\meassurement.txt", buffer);

  s8 line = file_iter_next(&line_arena, iter, '\n');
  while (line.len > 0) {
    // use whole line
    s8_println(line);
    arena_reset(&line_arena);
    line = file_iter_next(&line_arena, iter, '\n');
  }

  file_iter_close(iter);

  arena_reset(&input_data);
  arena_reset(&line_arena);

  iter = arena_alloc(&input_data, FileIter);
  buffer = s8_empty(&input_data, input_data.cap - input_data.offset);

  file_init_iter(iter, "E:\\repos\\C-Code\\BillionRows\\meassurement.txt", buffer);

  b32 run = 1;
  hashmap *map = { 0 };

  while (run)
  {
    s8 name = file_iter_next(&line_arena, iter, ';');
    s8 temp_s = file_iter_next(&line_arena, iter, '\n');
    if (name.len == 0 || temp_s.len == 0)
    {
      break;
    }

    f64 temp;
    s8_try_parse_f64(temp_s, &temp);
    f64* val_ptr = upsert(&map, name, &perm);
    
    *val_ptr = temp;
    s8_print(name);
    s8_print(s8_from_literal(" "));
    s8_println(temp_s);

   

    arena_reset(&line_arena);
  }
}