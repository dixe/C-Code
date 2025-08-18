#include <stdio.h>
#include "arena.h"
#include "s_string.h"
#include "custom_types.h"
#include "arena.h"
#include "file.h"
#include "hashmap.h"
#include "unicode.h"

typedef struct {
  f64 sum;
  i64 count;
} Data;


void print_res(s8 key, Data* data)
{

}

int main()
{
  Arena unicode_arena = arena_create(1024 * 1024);

  isize chunk_size = 256;
  Arena input_data = arena_create(chunk_size);

  FileIter* iter = arena_alloc(&input_data, FileIter);
  s8 buffer = s8_empty(&input_data, input_data.cap - input_data.offset);


  file_init_iter(iter, "E:\\repos\\C-Code\\unicode\\UDC\\UnicodeData.txt", buffer);
  UnicodeDataArr unicode_data = unicode_load_data_from_file(unicode_arena, iter);

  u8 data[] = { 0xE2, 0x9D, 0xA4 };
  s8 utf8 = s8_from_bytes((u8*)data, sizeof(data));

  s8_println(utf8);







  arena_reset(&input_data);

  iter = arena_alloc(&input_data, FileIter);


  isize line_size = 256;
  Arena line_arena = arena_create(line_size);
  
  Arena perm = arena_create(32);

  file_init_iter(iter, "E:\\repos\\C-Code\\BillionRows\\meassurement.txt", buffer);

  s8 line = file_iter_next(&line_arena, iter, '\n');
  while (line.byte_len > 0) {
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
  HashMapTrie *map = { 0 };

  while (run)
  {
    s8 name = file_iter_next(&line_arena, iter, ';');
    s8 temp_s = file_iter_next(&line_arena, iter, '\n');
    if (name.byte_len == 0 || temp_s.byte_len == 0)
    {
      break;
    }

    b32 has_val = hmt_contains(&map, name);
    f64 temp;
    s8_try_parse_f64(temp_s, &temp);
    Data* val_ptr = hmt_insert_get(&map, name, Data, &perm);
    

    has_val = hmt_contains(&map, name);

    val_ptr->sum += temp;
    val_ptr->count += 1;

    s8_print(name);
    s8_print(s8_from_literal(" "));
    s8_println(temp_s); 

    arena_reset(&line_arena);
  }

  hmt_iter(&map, print_res);
  // iter results 
}