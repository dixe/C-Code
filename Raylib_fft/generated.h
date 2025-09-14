/// Generated file do not modify, can and will change on build
#pragma once
#include "custom_types.h"
#include "arena.h"
#include "types.h"

typedef struct {
  isize count;
  isize capacity;
   Complex* data;
}  Sequence;


 Sequence  Sequence_empty(Arena* arena, isize capacity);

void  Sequence_add(Arena* arena,  Sequence* s,  Complex c);;


typedef struct {
  isize count;
  isize capacity;
   double* data;
}  DoubleArr;


 DoubleArr  DoubleArr_empty(Arena* arena, isize capacity);

void  DoubleArr_add(Arena* arena,  DoubleArr* s,  double c);;

