/// Generated file do not modify, can and will change on build
#include "generated.h"
#include "dyn_array.h"

Sequence Sequence_empty(Arena* arena,isize capacity)
{
  Sequence res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena, Complex, capacity);
  return res;

}

void Sequence_add(Arena* arena, Sequence* arr, Complex elm)
{
  arr->data = arr_grow(arena, arr->count, &arr->capacity, sizeof(Complex), arr->data);
  arr->data[arr->count] = elm;
  arr->count++;
};

f64Arr f64Arr_empty(Arena* arena,isize capacity)
{
  f64Arr res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena, f64, capacity);
  return res;

}

void f64Arr_add(Arena* arena, f64Arr* arr, f64 elm)
{
  arr->data = arr_grow(arena, arr->count, &arr->capacity, sizeof(f64), arr->data);
  arr->data[arr->count] = elm;
  arr->count++;
};

