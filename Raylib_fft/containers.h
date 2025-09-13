#pragma once

#include "custom_types.h"
#include "arena.h"
#include "types.h"


Sequence sequence_empty(Arena* arena, isize capacity);

void sequence_add(Arena* arena, Sequence* s, Complex c);