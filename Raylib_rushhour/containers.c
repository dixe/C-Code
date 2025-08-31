#include "containers.h"
#include "dyn_array.h"

void moves_add(Arena* arena, Moves* moves, Move* new_move)
{
  moves->data = arr_grow(arena, moves->count, &moves->capacity, sizeof(Move), moves->data);

  moves->data[moves->count] = *new_move;
  moves->count++;
}