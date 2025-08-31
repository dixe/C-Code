#pragma once 
#include "custom_types.h"
#pragma warning(push,0)
#pragma warning (disable : 4996 6031) // ignore fopen insecure in raylib
#include "raylib.h"
#pragma warning(pop)
#include "hashmap.h"


#define GRIDW 6

// Bricks are represented by an unique id. All bricks are defined by their anchor in the top/left
// Bricks are 1 tile wide and X tile long.
// Bricks can either move vertical or horizontal
typedef enum {
  HORIZONTAL,
  VERTICAL
} DIRECTION;

typedef struct {
  u8 brick_id;
  Vector2 starting_pos;
  Vector2 current_pos;
} MovingBrick;

typedef struct {
  u8 brick_id;
  i32 top_row;
  i32 left_col;
  Color color;
  DIRECTION dir;
  i32 len;
} Brick;

typedef struct {
  Brick data[GRIDW * GRIDW];
  u8 count;
} Bricks;


typedef struct
{
  u8 data[GRIDW * GRIDW];
} BoardState;

typedef struct {
  Bricks bricks;
  BoardState board;
} GameState;

typedef HashMapTrie BoardGraph;// key = boardstate wrapped in s8, value is pointer to array of Move we can access from here 



typedef struct {
  BoardState board;
  u8 brick_id;
  i32 horizontal;
  i32 vertical;
  // used for graphs
  s8 from_key;  
  i32 depth;
} Move;

typedef struct {
  isize count;
  isize capacity;
  Move* data;
} Moves;


typedef struct {
  BoardState initial_board;
  Moves moves;
  isize current_index;
} Solution;

typedef struct {
  Solution longest_puzzle;
  BoardGraph* all_moves;
} AllMoves;

