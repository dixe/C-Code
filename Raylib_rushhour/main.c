#include "raylib.h"
#include "arena.h"
#include "hashmap.h"
#pragma warning (disable : 4996) // ignore fopen insecure in raylib
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const i32 screenWidth = 1200;
const i32 screenHeight = 800;
i32 tileW = 80;
i32 brick_padding = 3;




u8 next_brick_id = 1;
#define GRIDW 6

// Bricks are represented by an unique id. All bricks are defined by their anchor in the top/left
// Bricks are 1 tile wide and X tile long.
// Bricks can either move vertical or horizontal
typedef enum  {
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
  isize count;
} Bricks;


typedef struct
{
  u8 data[GRIDW * GRIDW];
} BoardState;

typedef struct {
  Bricks bricks;
  BoardState board;
} GameState;

#define DEFARRAY(t, n) typedef struct {\
isize count; \
t* data; \
} n;

DEFARRAY(BoardState, Moves)


i32 anchor_width = 14;
b32 MoveGrid(Vector2 mouse, b32 resizing);
MovingBrick MoveBrick(Vector2 mouse, GameState* bricks, MovingBrick moving_brick);
void DrawBoard(GameState game, MovingBrick moving_brick);
void AddBrick(GameState* game, i32 left_col, i32 top_row, i32 len, DIRECTION dir, Color color);
b32 IsWinningState(Bricks bricks);
Rectangle BrickRect(Brick b);

Rectangle TileRect(i32 col, i32 row, i32 horizontal_tile, i32 vertical_tiles);
Moves FindMoves(Arena* a, GameState game);



Vector2 base_anchor;


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

  b32 moving_grid = 0;
  MovingBrick moving_brick = { 0 };
  base_anchor = (Vector2){ 200 + 10, 60};
  // See https://www.youtube.com/watch?v=dBlSmg5T13M
  // Initialization
  //--------------------------------------------------------------------------------------  

  InitWindow(screenWidth, screenHeight, "Rush hour");
  
  SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  Arena game_arena = arena_create(1024);
  
  GameState game = { 0 };
  
  // TARGET PIECE
  AddBrick(&game, 1, 2, 2, HORIZONTAL, GREEN);
  AddBrick(&game, 0, 0, 2, HORIZONTAL, BLUE);

  AddBrick(&game, 0, 1, 4, HORIZONTAL, GRAY);
  AddBrick(&game, 0, 2, 2, VERTICAL, YELLOW);
  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {
    Vector2 mouse = GetMousePosition();

    moving_grid = MoveGrid(mouse, moving_grid);
    moving_brick = MoveBrick(mouse, &game, moving_brick);

    if (GuiButton((Rectangle) { 10, 10, 80, 50 }, "Find moves"))
    {
      
      Moves moves = FindMoves(&game_arena, game);
    }



    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(BLACK);
    DrawBoard(game, moving_brick);

    if (IsWinningState(game.bricks))
    {
      DrawText("SUCCESS", 400, 40, 30, GREEN);
    }

    EndDrawing();

  }

  CloseWindow();
  return 0;
}



Moves FindMoves(Arena* a, GameState game)
{
  Moves res = { 0 };

  for (i32 i = 0; i < game.bricks.count; i++)
  {
    // move brick 1 step along dir, in both direction;
    
  }
  return res;
}

void UpdateBoard(GameState* game)
{
  memset(game->board.data, 0, GRIDW * GRIDW);

  for (i32 idx = 0; idx < game->bricks.count; idx++)
  {
    for (i32 i = 0; i < game->bricks.data[idx].len; i++)
    {
      Brick b = game->bricks.data[idx];
      i32 row = b.top_row + (VERTICAL == b.dir) * i;
      i32 col = b.left_col + (HORIZONTAL == b.dir) * i;
      game->board.data[row * GRIDW + col] = b.brick_id;
    }
  }
}

void AddBrick(GameState* game, i32 left_col, i32 top_row, i32 len, DIRECTION dir, Color color)
{
  Brick* brick = &game->bricks.data[game->bricks.count];
  brick->brick_id = next_brick_id;
  brick->color = color;
  brick->left_col = left_col;
  brick->top_row = top_row;
  brick->len = len;
  brick->dir = dir;
  game->bricks.count += 1;

  next_brick_id += 1;

  UpdateBoard(game);
}



b32 IsMoveValid(GameState* game, Brick b, i32 new_row, i32 new_col) {
  b32 valid = 1;
  for (i32 j = 0; j <b.len; j++)
  {
    i32 row = new_row + (VERTICAL == b.dir) * j;
    i32 col = new_col + (HORIZONTAL == b.dir) * j;
    u8 tile_id = game->board.data[row * GRIDW + col];
    valid &= (row < GRIDW&& row >= 0 && col >= 0 && row < GRIDW) && tile_id == 0 || tile_id == b.brick_id;
  }
  return valid;
}

MovingBrick MoveBrick(Vector2 mouse, GameState* game, MovingBrick moving_brick) {


  if (moving_brick.brick_id != 0)
  {
    moving_brick.current_pos = mouse;   

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

      // find brick
      for (i32 i = 0; i < game->bricks.count; i++)
      {
        if (game->bricks.data[i].brick_id == moving_brick.brick_id)
        {
          i32 mouse_start_col = (moving_brick.starting_pos.x - base_anchor.x) / tileW;
          i32 mouse_end_col= (moving_brick.current_pos.x - base_anchor.x) / tileW;
          i32 mouse_start_row = (moving_brick.starting_pos.y - base_anchor.y) / tileW;
          i32 mouse_end_row = (moving_brick.current_pos.y - base_anchor.y) / tileW;

          // validate move before assignment
          i32 new_col = game->bricks.data[i].left_col + (game->bricks.data[i].dir == HORIZONTAL) * (mouse_end_col - mouse_start_col);
          i32 new_row = game->bricks.data[i].top_row + (game->bricks.data[i].dir == VERTICAL) * (mouse_end_row - mouse_start_row);
          
          b32 valid = IsMoveValid(game, game->bricks.data[i], new_row, new_col);
          
          if(valid)
          {
            game->bricks.data[i].top_row = new_row;;
            game->bricks.data[i].left_col = new_col;
            UpdateBoard(game);
          }

        }
      }

      moving_brick = (MovingBrick){ 0 };
    }

  }
  else
  {
    // check if we clicked in a brick
    for (i32 i = 0; i < game->bricks.count; i++)
    {

      Rectangle brick_rect = BrickRect(game->bricks.data[i]);

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, brick_rect))
      {
        moving_brick.brick_id = game->bricks.data[i].brick_id;
        moving_brick.starting_pos = mouse;
        moving_brick.current_pos = mouse;
        break;
      }
    }    
  }

  return moving_brick;
}

b32 MoveGrid(Vector2 mouse, b32 resizing) {

  if (resizing)
  {
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) resizing = false;

    base_anchor.x = mouse.x + anchor_width/2;
    base_anchor.y = mouse.y + anchor_width / 2;
  }
  else
  {
    // Check if we're moving_grid
    Rectangle anchor = (Rectangle){ base_anchor.x - anchor_width, base_anchor.y - anchor_width, anchor_width, anchor_width };
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, anchor)) resizing = true;
  }
  return resizing;
}

void DrawBoard(GameState game, MovingBrick moving_brick)
{

  // draw base_anchor

  DrawRectangle(base_anchor.x - anchor_width, base_anchor.y - anchor_width, anchor_width, anchor_width, MAROON);

  // draw grid
  i32 baseX = base_anchor.x;
  i32 baseY = base_anchor.y;
  // assume square grid
  for (i32 i = 0; i <= GRIDW; i++)
  {
    // vertical lines
    DrawLine(baseX + i * tileW, baseY , baseX + i * tileW, baseY + tileW * GRIDW, RAYWHITE);

    // horizontal lines
    DrawLine(baseX, baseY + i * tileW, baseX + tileW * GRIDW, baseY + i * tileW, RAYWHITE);
  }

  // FINISH LINE
  DrawLine(baseX + GRIDW * tileW, baseY + 2 * tileW, baseX + GRIDW * tileW, baseY + tileW * 3, GREEN);

  // draw bricks
  for (i32 i = 0; i < game.bricks.count; i++)
  {
    Rectangle br = BrickRect(game.bricks.data[i], baseX, baseY, tileW, brick_padding);

    i32 x_offset = 0;
    i32 y_offset = 0;
    if (moving_brick.brick_id == game.bricks.data[i].brick_id)
    {
      x_offset = (game.bricks.data[i].dir == HORIZONTAL) * (moving_brick.current_pos.x - moving_brick.starting_pos.x);
      y_offset = (game.bricks.data[i].dir == VERTICAL) * (moving_brick.current_pos.y - moving_brick.starting_pos.y);
    }

    DrawRectangle(br.x + x_offset, br.y + y_offset, br.width, br.height, game.bricks.data[i].color);
  }

  for (i32 i = 0; i < GRIDW; i++)
  {
    for (i32 j = 0; j < GRIDW; j++)
    {
      char str[6] = { 0 };
      u8 id = game.board.data[i * GRIDW + j];
      snprintf(str, 6, "%d", id);
      Rectangle tile_rect = TileRect(i, j, 1, 1);
      Color color = id == 0 ? WHITE : BLACK;
      DrawText(str, tile_rect.x + tile_rect.width / 2 - 5, tile_rect.y + tile_rect.height / 2 - 5, 20, color);
    }
  }

}

b32 IsWinningState(Bricks bricks)
{
  if (bricks.count == 0)
  {
    return 0;
  }

  return bricks.data[0].left_col == 4;
}


Rectangle TileRect(i32 row, i32 col, i32 horizontal_tile, i32 vertical_tiles) {
  i32 base_x = base_anchor.x;
  i32 base_y = base_anchor.y;
  i32 x = base_x + col * tileW + brick_padding;
  i32 y = base_y + row * tileW + brick_padding;
  i32 width = horizontal_tile * tileW; 
  i32 height = vertical_tiles * tileW; 

  return (Rectangle) { x, y, width - brick_padding * 2, height - brick_padding * 2 };
}

Rectangle BrickRect(Brick b) {

  return TileRect(b.top_row, b.left_col, 1 + (b.dir == HORIZONTAL) * (b.len - 1), 1 + (b.dir == VERTICAL) * (b.len - 1));
}