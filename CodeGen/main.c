#include <stdio.h>
#include <stdlib.h>
#include "file.h"

Arena templates_arena;
Arena data_arena;
s8 array_template_c = { 0 };
s8 array_template_h = { 0 };

int exists(const char* fname)
{

  FILE* file;
  errno_t err = fopen_s(&file, fname, "r");
  if (err == 0)
  {
    fclose(file);
    return 1;
  }
  return 0;
}

void add_include(s8* header_output,  s8Slice next_line)
{
  s8_append(&data_arena, header_output, next_line.str);
  s8_append(&data_arena, header_output, s8_from_literal("\n"));
}

void load_template(s8 *dest, char* path) {
  // load template 
  FILE* fp;  
  errno_t err = fopen_s(&fp, path, "r");
  if (err != 0)
  {
    printf("Error opening array template file");
    exit(32);
  }
  *dest = file_read_all(&templates_arena, fp);  
  fclose(fp);
}

void load_array_template()
{
  load_template(&array_template_c, "E:\\repos\\C-Code\\CodeGen\\array_template.c");
  load_template(&array_template_h, "E:\\repos\\C-Code\\CodeGen\\array_template.h");
}


void add_array_code(s8* code_output, s8Slice type_name, s8Slice struct_name)
{

  if (array_template_h.data == 0)
  {
    load_array_template();
  }

  isize arena_offset = templates_arena.offset;
  s8 replaced = s8_replace(&templates_arena, array_template_c, s8_from_literal("TYPENAME"), type_name.str);
  // could be replace_inline
  replaced = s8_replace(&templates_arena, replaced, s8_from_literal("STRUCTNAME"), struct_name.str);


  s8_append(&data_arena, code_output, replaced);
  s8_append(&data_arena, code_output, s8_from_literal(";\n\n"));

  // fake reset arena
  templates_arena.offset = arena_offset;

}

void add_array_header(s8* header_output, s8Slice type_name, s8Slice struct_name)
{
  
  if (array_template_h.data == 0)
  {
    load_array_template();
  }

  isize arena_offset = templates_arena.offset;
  s8 replaced = s8_replace(&templates_arena, array_template_h, s8_from_literal("TYPENAME"), type_name.str);  
  // could be replace_inline
  replaced = s8_replace(&templates_arena, replaced, s8_from_literal("STRUCTNAME"), struct_name.str);
  

  s8_append(&data_arena, header_output, replaced);
  s8_append(&data_arena, header_output, s8_from_literal(";\n\n"));

  // fake reset arena
  templates_arena.offset = arena_offset;

}
//
//void add_array_header(s8* header_output, s8Slice type_name, s8Slice struct_name)
//{
//  if (array_template_h.data == 0)
//  {
//    load_array_template();
//  }
//
//  isize arena_offset = templates_arena.offset;
//  s8 replaced = s8_replace(&templates_arena, array_template_c, s8_from_literal("TYPENAME"), type_name.str);
//  // could be replace_inline
//  replaced = s8_replace(&templates_arena, replaced, s8_from_literal("STRUCTNAME"), struct_name.str);
//
//
//  s8_append(&data_arena, code_output, replaced);
//  s8_append(&data_arena, code_output, s8_from_literal(";\n\n"));
//
//  // fake reset arena
//  templates_arena.offset = arena_offset;
//
//
//
//  s8 struct_start = s8_from_literal("typedef struct {\n  isize count;\n  isize capacity;\n ");
//  s8_append(&data_arena, header_output, struct_start);
//  s8_append(&data_arena, header_output, type_name.str);
//  s8_append(&data_arena, header_output, s8_from_literal("* data;\n}"));
//  s8_append(&data_arena, header_output, struct_name.str);
//  s8_append(&data_arena, header_output, s8_from_literal(";\n\n"));
//}

void add_array(s8* header_output, s8* code_output, s8Slice next_line)
{
  isize typename_idx = s8_find_char(0, next_line.str, ' ');
  isize structname_idx = s8_find_char(typename_idx + 1, next_line.str, ' ');
  isize structname_end_idx = s8_find_char(structname_idx + 1, next_line.str, '\0');  

  if (structname_end_idx == -1)
  {
    structname_end_idx = next_line.str.byte_len;
  }

  s8Slice type_name = s8_subslice(next_line.str, typename_idx, structname_idx);
  s8Slice struct_name = s8_subslice(next_line.str, structname_idx, structname_end_idx);
  // remove space in from
  type_name.str.data += 1;
  type_name.str.byte_len -= 1;
  struct_name.str.data += 1;
  struct_name.str.byte_len -= 1;

  add_array_header(header_output, type_name, struct_name);
  add_array_code(code_output, type_name, struct_name);


}

void parse_line(s8* header_output, s8* code_output, s8Slice next_line)
{  
  if(s8_starts_with(next_line.str, s8_from_c_str("#")))
  {
    add_include(header_output, next_line);
  }
  s8 array = s8_from_c_str("Array");
  if (s8_starts_with(next_line.str, array))
  {
    add_array(header_output, code_output, next_line);
  }
}

int main(int argc, char* argv[]) {

  if (argc != 4)
  {
    printf("\nUsage: CodeGen genFilePath headerpath codePath\n\n");
    return 1;
  }

  char* genFilePath = argv[1];
  char* headerpath = argv[2];
  char* codePath = argv[3];

  if (!exists(genFilePath))
  {
    printf("Canno open GenFilePath, might not exsit '%s', genFilePath", genFilePath);
    return 2;
  }
  
  data_arena = arena_create(1024);
  templates_arena = arena_create(1024);
  
  FILE* file;
  errno_t err = fopen_s(&file, genFilePath, "r");

  // read input file
  s8 gen_content = file_read_all(&data_arena, file);
  if (file != 0) 
  {
    fclose(file);
  }

  s8 header_output = s8_from_literal("/// Generated file do not modify, can and will change on build\n");
  s8_append(&data_arena, &header_output, s8_from_literal("#pragma once\n"));
  s8_append(&data_arena, &header_output, s8_from_literal("#include \"custom_types.h\"\n"));
  s8_append(&data_arena, &header_output, s8_from_literal("#include \"arena.h\"\n"));

  s8 code_output = s8_from_literal("/// Generated file do not modify, can and will change on build\n");
  s8_append(&data_arena, &code_output, s8_from_literal("#include \"generated.h\"\n"));
  s8_append(&data_arena, &code_output, s8_from_literal("#include \"dyn_array.h\"\n\n"));

  s8Slice next_line = s8_next_line_slice(gen_content, 0);

  while (next_line.offset_in_original < gen_content.byte_len || next_line.str.byte_len > 0)
  {
    parse_line(&header_output, &code_output, next_line);
    next_line = s8_next_line_slice(gen_content, next_line.offset_in_original + 1);
  }

  // write generated header file
  err = fopen_s(&file, headerpath, "w");
  if (file != 0)
  {
    fwrite(header_output.data, sizeof(u8), header_output.byte_len, file);
    fclose(file);
  }
  
  // write generated code file
  err = fopen_s(&file, codePath, "w");
  if (file != 0)
  {
    fwrite(code_output.data, sizeof(u8), code_output.byte_len, file);
    fclose(file);
  }



}