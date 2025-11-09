/// Generated file do not modify, can and will change on build
#include "generated.h"
#include "dyn_array.h"

#include <string.h>

Segments Segments_empty(Arena* arena, isize capacity)
{
  Segments res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena, Segment, capacity);
  return res;

}

void Segments_add(Arena* arena, Segments* arr, Segment elm)
{
  arr->data = arr_grow(arena, arr->count, &arr->capacity, sizeof(Segment), arr->data);
  arr->data[arr->count] = elm;
  arr->count++;
}

Segments Segments_clone(Arena* arena, Segments s)
{
  Segments res = Segments_empty(arena, s.capacity);
  memcpy(res.data, s.data, s.count * sizeof(Segment));

  res.count = s.count;
  return res;
}
;

VecArr VecArr_empty(Arena* arena, isize capacity)
{
  VecArr res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena, Vector2, capacity);
  return res;

}

void VecArr_add(Arena* arena, VecArr* arr, Vector2 elm)
{
  arr->data = arr_grow(arena, arr->count, &arr->capacity, sizeof(Vector2), arr->data);
  arr->data[arr->count] = elm;
  arr->count++;
}

VecArr VecArr_clone(Arena* arena, VecArr s)
{
  VecArr res = VecArr_empty(arena, s.capacity);
  memcpy(res.data, s.data, s.count * sizeof(Vector2));

  res.count = s.count;
  return res;
}
;

