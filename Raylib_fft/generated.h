/// Generated file do not modify, can and will change on build
#pragma once
#include "custom_types.h"
#include "arena.h"
#include "types.h"¬
typedef struct {
  isize count;
  isize capacity;
  Complex* data;
} Sequence;


Sequence Sequence_empty(Arena* arena, isize capacity);

void Sequence_add(Arena* arena, Sequence* s, Complex c);

Sequence Sequence_clone(Arena* arena, Sequence s);;


typedef struct {
  isize count;
  isize capacity;
  f64* data;
} f64Arr;


f64Arr f64Arr_empty(Arena* arena, isize capacity);

void f64Arr_add(Arena* arena, f64Arr* s, f64 c);

f64Arr f64Arr_clone(Arena* arena, f64Arr s);;


typedef struct {
  isize count;
  isize capacity;
  PeakFreq* data;
} PeakFreqArray;


PeakFreqArray PeakFreqArray_empty(Arena* arena, isize capacity);

void PeakFreqArray_add(Arena* arena, PeakFreqArray* s, PeakFreq c);

PeakFreqArray PeakFreqArray_clone(Arena* arena, PeakFreqArray s);;

