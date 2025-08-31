#include "dyn_array.h"
#include "arena.h"
#include <string.h>  // For memcpy
#include <stdlib.h>

void* arr_grow(Arena* a, isize count, isize* capacity, isize data_size, void* data_ptr)
{
  if (count < *capacity)
  {
    return data_ptr;
  }

  if (data_size == 0)
  {
    exit(23);
  }

  // grow by 2x?
  isize new_capacity = *capacity == 0 ? 2 : *capacity * 2;
  void* new_data_ptr = arena_realloc(a, data_ptr, *capacity * data_size, new_capacity * data_size);

  // maybe copy prev data to new location
  if (count > 0)
  {
    memcpy(new_data_ptr, data_ptr, *capacity * data_size);
  }
  *capacity = new_capacity;

  return new_data_ptr;
}