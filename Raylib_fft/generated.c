/// Generated file do not modify, can and will change on build
#include "generated.h"
#include "dyn_array.h"

#include <string.h>

Sequence Sequence_empty(Arena* arena, isize capacity)
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
}

Sequence Sequence_clone(Arena* arena, Sequence s)
{
  Sequence res = Sequence_empty(arena, s.capacity);
  memcpy(res.data, s.data, s.count * sizeof(Complex));

  res.count = s.count;
  return res;
}
;

f64Arr f64Arr_empty(Arena* arena, isize capacity)
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
}

f64Arr f64Arr_clone(Arena* arena, f64Arr s)
{
  f64Arr res = f64Arr_empty(arena, s.capacity);
  memcpy(res.data, s.data, s.count * sizeof(f64));

  res.count = s.count;
  return res;
}
;

PeakFreqArray PeakFreqArray_empty(Arena* arena, isize capacity)
{
  PeakFreqArray res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena, PeakFreq, capacity);
  return res;

}

void PeakFreqArray_add(Arena* arena, PeakFreqArray* arr, PeakFreq elm)
{
  arr->data = arr_grow(arena, arr->count, &arr->capacity, sizeof(PeakFreq), arr->data);
  arr->data[arr->count] = elm;
  arr->count++;
}

PeakFreqArray PeakFreqArray_clone(Arena* arena, PeakFreqArray s)
{
  PeakFreqArray res = PeakFreqArray_empty(arena, s.capacity);
  memcpy(res.data, s.data, s.count * sizeof(PeakFreq));

  res.count = s.count;
  return res;
}
;

