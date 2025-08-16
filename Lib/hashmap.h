#pragma once

#include "custom_types.h"
#include "s_string.h"
#include "arena.h"

// Fixed map 
// https://nullprogram.com/blog/2022/08/08/

typedef struct hashmap {
  struct hashmap *child[4];
  s8 key; // pointer or value? kinda the same, since s8 has a pointer to data
  void* value;
} hashmap;




b32 hm_contains(hashmap** map, s8 key);

void* _upsert(hashmap **map, s8 key, Arena* a);
