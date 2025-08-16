#include "hashmap.h"




u64 hash(s8 s)
{
  u64 h = 0x100;
  for (u64 i = 0; i < (u64)s.len; i++)
  {
    h ^= s.data[i];
    h *= 1111111111111111111u;
  }
  return h;
}

void* upsert(hashmap **map, s8 key, Arena* a)
{

  u64 h = hash(key);
  
  for (; *map; h <<= 2)
  {    
    b32 eq = s8_equals(key, (*map)->key);
    if (eq)
    {
      return &(*map)->value;
    }
    map = &(*map)->child[h >> 62];
  }

  // map is 0
  if (a == 0)
  {
    return 0;
  }
  hashmap* new_map = arena_alloc(a, hashmap);
  
  memset(new_map, 0, sizeof(hashmap));
  (*map) = new_map;
  // make a copy of the key into the hashmap arena
  (*map)->key = s8_empty(a, key.len);
  s8_append(a, &(*map)->key, key, 0, key.len);
  return &(*map)->value;
  

}