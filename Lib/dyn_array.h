#pragma once 

#include "arena.h"
#include "custom_types.h"


typedef struct {
  isize count;
  isize capacity;
  isize data_size;
} ArrayHeader;


//#define ARR_DEFARRAY(t, n) typedef struct {\
//isize count; \
//isize capacity; \
//isize data_size; \
//t* data; \
//} n;



void* arr_grow(Arena* a, isize count, isize* capacity, isize data_size, void* data_ptr);
//void arr_add_intern(Arena* a, ArrayHeader* arr, void* new_data, isize new_data_size);

//#define arr_add(arena, arr, data, data_type) arr_add_intern(arena, &arr, &data, sizeof(data_type))

/// <summary>
/// Concat s1 and s2 into a new string allocated in arena
/// s1 and s2 are not touched
/// Use append to modify s1
/// </summary>
/// <param name="a"></param>
/// <param name="s1"></param>
/// <param name="s2"></param>
/// <returns></returns>
//s8 arr_concat(Arena* a, s8 s1, s8 s2);

/// <summary>
/// Appends src[src_start;src_end] to dest. If dest has capacity, resuse memory, otherwise realloc in Arena a
/// </summary>
/// <param name="a"></param>
/// <param name="dest"></param>
/// <param name="src"></param>
/// <param name="src_start"></param>
/// <param name="src_end"></param>
//DynArray* arr_add(Arena* a, DynArray* dest, void* data, isize data_size);