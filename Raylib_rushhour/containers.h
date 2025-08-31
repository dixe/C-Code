#pragma once 

#include "custom_types.h"
#include "arena.h"
#include "types.h"


Moves moves_empty(Arena* arena, isize capacity);

void moves_add(Arena* arena, Moves* moves, Move *new_move);