/// Generated file do not modify, can and will change on build
#include "generated.h"
#include "dyn_array.h"

 Sequence  Sequence_empty(Arena* arena,isize capacity)
{
   Sequence res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena,  Complex, capacity);
  return res;

}

void  Sequence_add(Arena* arena,  Sequence* arr,  Complex elm)
{
  arr->data = arr_grow(arena, arr->count, &arr->capacity, sizeof( Complex), arr->data);
  arr->data[arr->count] = elm;
  arr->count++;
};

 DoubleArr  DoubleArr_empty(Arena* arena,isize capacity)
{
   DoubleArr res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena,  double, capacity);
  return res;

}

void  DoubleArr_add(Arena* arena,  DoubleArr* arr,  double elm)
{
  arr->data = arr_grow(arena, arr->count, &arr->capacity, sizeof( double), arr->data);
  arr->data[arr->count] = elm;
  arr->count++;
};

