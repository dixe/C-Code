#include "dyn_array.h"
#include "arena.h"
#include <string.h>  // For memcpy



void arr_add_intern(Arena* a, ArrayHeader *arr,  void* new_data, isize new_data_size)
{
  if (arr->data_size == 0)
  {
    arr->data_size = new_data_size;
  }

  if (new_data_size != arr->data_size)
  {
    // Error wrong array type
    // hard fail exit
    exit(55);
  }

  if (arr->count >= arr->capacity)
  {

    // grow by 2x?
    isize new_size = arr->capacity == 0 ? 2 : arr->capacity * 2;
    void* new_data_ptr = arena_realloc(a, arr->data, arr->capacity * arr->data_size, new_size * arr->data_size);

    // maybe copy prev data to new location
    if (arr->count > 0)
    {
      memcpy(arr->data, new_data_ptr, arr->capacity * arr->data_size);
    }

    arr->data = new_data_ptr;
    arr->capacity = new_size;
  }

  // add new item into array
  memcpy((&((u8*)arr->data)[arr->count * arr->data_size]), new_data, new_data_size);
  arr->count += 1; 
}