/// Generated file do not modify, can and will change on build
#pragma once
#include "custom_types.h"
#include "arena.h"
#include "types.h"
#include "raylib.h"

typedef struct {
  isize count;
  isize capacity;
  Segment* data;
} Segments;


Segments Segments_empty(Arena* arena, isize capacity);

void Segments_add(Arena* arena, Segments* s, Segment c);

Segments Segments_clone(Arena* arena, Segments s);;


typedef struct {
  isize count;
  isize capacity;
  Vector2* data;
} VecArr;


VecArr VecArr_empty(Arena* arena, isize capacity);

void VecArr_add(Arena* arena, VecArr* s, Vector2 c);

VecArr VecArr_clone(Arena* arena, VecArr s);;

