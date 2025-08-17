#pragma once

#include "custom_types.h"
#include "s_string.h"
#include "arena.h"
#include <math.h>
// Fixed map 
// https://nullprogram.com/blog/2022/08/08/

#ifndef HASHMAPTRIE_POT
#define HASHMAPTRIE_POT 5 // used to set how many children a HasMapTrie should have, defeau
#endif // DEBUG
/// <summary>
/// Hashmap implemented as a trie with 2^HASHMAPTRIE_POT child per node
/// Supports dynamic insert of elements, without reallocating old elements
/// stores spaces for the value after the node.
/// </summary>
typedef struct HashMapTrie {
  struct HashMapTrie*child[1 <<HASHMAPTRIE_POT];
  s8 key; // pointer or value? kinda the same, since s8 has a pointer to data
  void* value;
} HashMapTrie;


/// <summary>
/// return pointer to value if key is in map, 0 if not
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns>Void pointer to value</returns>
void* hmt_get(HashMapTrie** map, s8 key);

/// <summary>
/// Return 1 if key is present in dict, 0 if not
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
b32 hmt_contains(HashMapTrie** map, s8 key);


/// <summary>
/// See hmt_insert_get for clean interface
/// Insert and gets key, if key exists a pointer to value is returned
/// If key does not exist and arena is 0, null pointer is return. This is equivilant to hmt_contains
/// if key does not exist and arena is not null a new node is allocated in the area, and pointer to the value si returned
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <param name="val_size">Size of data to be stored. Should be the same for all calls for the same map</param>
/// <param name="a">Arena to store data in.</param>
/// <returns>Pointer to the value</returns>
void* _hmt_upsert(HashMapTrie** map, s8 key, isize val_size, Arena* a);

/// <summary>
/// Recursive iteration depth first
/// </summary>
/// <param name="map"></param>
/// <param name="fptr"></param>
void hmt_iter(HashMapTrie** map, void (*fptr)(s8, void*));

///Wrapper for upsert
#define hmt_insert_get(map, key, t, a) (t *)_hmt_upsert(map, key, sizeof(t), a);