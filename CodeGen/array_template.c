STRUCTNAME STRUCTNAME_empty(Arena* arena, isize capacity)
{
  STRUCTNAME res = { 0 };
  res.capacity = capacity;
  res.data = arena_alloc(arena, TYPENAME, capacity);
  return res;

}

void STRUCTNAME_add(Arena* arena, STRUCTNAME* arr, TYPENAME elm)
{
  arr->data = arr_grow(arena, arr->count, &arr->capacity, sizeof(TYPENAME), arr->data);
  arr->data[arr->count] = elm;
  arr->count++;
}

STRUCTNAME STRUCTNAME_clone(Arena* arena, STRUCTNAME s)
{
  STRUCTNAME res = STRUCTNAME_empty(arena, s.capacity);
  memcpy(res.data, s.data, s.count * sizeof(TYPENAME));

  res.count = s.count;
  return res;
}
