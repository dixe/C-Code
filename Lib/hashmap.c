#include "hashmap.h"
#include "s_string.h"

u64 hash(s8 s)
{
  u64 h = 0x100;
  for (u64 i = 0; i < (u64)s.byte_len; i++)
  {
    h ^= s.data[i];
    h *= 1111111111111111111u;
  }
  return h;
}


void* hmt_get(HashMapTrie** map, s8 key)
{
  return _hmt_upsert(map, key, 0, 0);
}


b32 hmt_contains(HashMapTrie** map, s8 key)
{
  return _hmt_upsert(map, key, 0, 0) != 0;
}

void* _hmt_upsert(HashMapTrie**map, s8 key, isize val_size, Arena* a)
{
  u64 h = hash(key);  
  for (; *map; h <<= HASHMAPTRIE_POT)
  {    
    b32 eq = s8_equals(key, (*map)->key);
    if (eq)
    {
      return &(*map)->value;
    }
    map = &(*map)->child[h >> (64 - HASHMAPTRIE_POT)];
  }

  // map is 0
  if (a == 0)
  {
    return 0;
  }

  // assume arena allocates in continious virtual address space
  HashMapTrie* new_map = arena_alloc(a, HashMapTrie, 1);
  if (val_size > sizeof(void*))
  {
    arena_alloc(a, u8, val_size - sizeof(void*) ); // maybe we just need val_size- sizeof_
  }
  else {
    val_size = 0;
  }
  memset(new_map, 0, sizeof(HashMapTrie) + val_size);
  (*map) = new_map;
  // make a copy of the key into the hashmap arena
  (*map)->key = s8_empty(a, key.byte_len);
  s8_append(a, &(*map)->key, key, 0, key.byte_len);
  return &(*map)->value;
}


void hmt_iter(HashMapTrie** map, void (*fptr)(s8, void*))
{
  // call fn with map.key and map.value  
  fptr((*map)->key, &(*map)->value);
  for (isize i = 0; i < 1 << HASHMAPTRIE_POT; i++)
  {
    HashMapTrie** child = &(*map)->child[i];
    if (*child == 0)
    {
      continue;
    }
    hmt_iter(child, fptr);
  }
}