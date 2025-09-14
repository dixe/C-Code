
typedef struct {
  isize count;
  isize capacity;
  TYPENAME* data;
} STRUCTNAME;


STRUCTNAME STRUCTNAME_empty(Arena* arena, isize capacity);

void STRUCTNAME_add(Arena* arena, STRUCTNAME* s, TYPENAME c);