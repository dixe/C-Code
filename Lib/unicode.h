#pragma once 
#include "s_string.h"
#include "custom_types.h"
#include "hashmap.h"
#include "file.h"



typedef struct {
  u32 code;
  s8 char_name;
  s8 gen_category;
  s8 canonical_category;
  s8 bidirectional_category;
  s8 decomb_map;
  // decimal, digit and numeric values ignores
  u8 mirrored;
  s8 unicode_1_name;
  u8 uppercase_mapping;
  u8 lowercase_mapping;
  u8 titlcase_mapping;
} UnicodeData;


typedef struct {
  UnicodeData* data;
  isize len;
} UnicodeDataArr;



s8 utf32_to_utf8(u32 code, Arena* a);



HashMapTrie unicode_load_data_from_file(Arena a, FileIter* fi);
