#include "containers.h"
#include "dyn_array.h"


Sequence sequence_empty(Arena* arena, isize capacity)
{
  Sequence res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena, Complex, capacity);
  return res;

}

void sequence_add(Arena* arena, Sequence* s, Complex c)
{
  s->data = arr_grow(arena, s->count, &s->capacity, sizeof(Complex), s->data);
  s->data[s->count] = c;
  s->count++;
}