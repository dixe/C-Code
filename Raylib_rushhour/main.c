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


Vector2 base_anchor;
i32 anchor_width = 14;
b32 MoveGrid(Vector2 mouse, b32 resizing);
MovingBrick MoveBrick(Vector2 mouse, Bricks* bricks, MovingBrick moving_brick);
void DrawBoard(Bricks bricks, MovingBrick moving_brick);
void AddBrick(Bricks* bricks, i32 left_col, i32 top_row, i32 len, DIRECTION dir, Color color);
Rectangle BrickRect(Brick b);
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

  b32 resizing = 0;
  MovingBrick moving_brick = { 0 };
  base_anchor = (Vector2){ 200 + 10, 60};
  // See https://www.youtube.com/watch?v=dBlSmg5T13M
  // Initialization
  //--------------------------------------------------------------------------------------  

  InitWindow(screenWidth, screenHeight, "Rush hour");
  
  SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  Arena game_arena = arena_create(1024);
  
  Bricks bricks = { 0 };
  
  // TARGET PIECE
  AddBrick(&bricks, 1, 2, 2, HORIZONTAL, GREEN);
  AddBrick(&bricks, 0, 0, 2, HORIZONTAL, BLUE);

  AddBrick(&bricks, 0, 1, 4, HORIZONTAL, GRAY);  
  AddBrick(&bricks, 0, 2, 2, VERTICAL, YELLOW);
  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {    
    Vector2 mouse = GetMousePosition();

    resizing = MoveGrid(mouse, resizing);
    moving_brick = MoveBrick(mouse, &bricks, moving_brick);

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(BLACK);
    DrawBoard(bricks, moving_brick);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

void AddBrick(Bricks* bricks, i32 left_col, i32 top_row, i32 len, DIRECTION dir, Color color)
{
  Brick* brick = &bricks->data[bricks->count];
  brick->brick_id = next_brick_id;
  brick->color = color;
  brick->left_col = left_col;
  brick->top_row = top_row;
  brick->len = len;
  brick->dir = dir;

  bricks->count += 1;

  next_brick_id += 1;
}


MovingBrick MoveBrick(Vector2 mouse, Bricks* bricks, MovingBrick moving_brick) {


  if (moving_brick.brick_id != 0)
  {
    moving_brick.current_pos = mouse;   

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

      // find brick
      for (i32 i = 0; i < bricks->count; i++)
      {
        if (bricks->data[i].brick_id == moving_brick.brick_id)
        {
          i32 mouse_start_col = (moving_brick.starting_pos.x - base_anchor.x) / tileW;
          i32 mouse_end_col= (moving_brick.current_pos.x - base_anchor.x) / tileW;
          i32 mouse_start_row = (moving_brick.starting_pos.y - base_anchor.y) / tileW;
          i32 mouse_end_row = (moving_brick.current_pos.y - base_anchor.y) / tileW;

          // validate move before assignment
          bricks->data[i].left_col += (bricks->data[i].dir == HORIZONTAL) * (mouse_end_col - mouse_start_col);
          bricks->data[i].top_row += (bricks->data[i].dir == VERTICAL) * (mouse_end_row - mouse_start_row);

        }
      }

      moving_brick = (MovingBrick){ 0 };
    }

  }
  else
  {
    // check if we clicked in a brick
    for (i32 i = 0; i < bricks->count; i++)
    {

      Rectangle brick_rect = BrickRect(bricks->data[i]);

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, brick_rect))
      {
        moving_brick.brick_id = bricks->data[i].brick_id;
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

    base_anchor.x = mouse.x - anchor_width/2;
    base_anchor.y = mouse.y - anchor_width / 2;
  }
  else
  {
    // Check if we're resizing
    Rectangle anchor = (Rectangle){ base_anchor.x - anchor_width, base_anchor.y - anchor_width, anchor_width, anchor_width };
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, anchor)) resizing = true;
  }
  return resizing;
}

void DrawBoard(Bricks bricks, MovingBrick moving_brick)
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
  for (i32 i = 0; i < bricks.count; i++)
  {
    Rectangle br = BrickRect(bricks.data[i], baseX, baseY, tileW, brick_padding);

    i32 x_offset = 0;
    i32 y_offset = 0;
    if (moving_brick.brick_id == bricks.data[i].brick_id)
    {
      x_offset = (bricks.data[i].dir == HORIZONTAL) * (moving_brick.current_pos.x - moving_brick.starting_pos.x);
      y_offset = (bricks.data[i].dir == VERTICAL) * (moving_brick.current_pos.y - moving_brick.starting_pos.y);
    }

    DrawRectangle(br.x + x_offset, br.y + y_offset, br.width, br.height, bricks.data[i].color);
  }

}

Rectangle BrickRect(Brick b) {
  i32 base_x = base_anchor.x;
  i32 base_y = base_anchor.y;
  i32 x = base_x + b.left_col * tileW + brick_padding;
  i32 y = base_y + b.top_row * tileW + brick_padding;
  i32 width = (1 + (b.dir == HORIZONTAL) * (b.len - 1)) * tileW;
  i32 height = (1 + (b.dir == VERTICAL) * (b.len - 1)) * tileW;

  return (Rectangle) { x, y, width - brick_padding * 2, height - brick_padding * 2 };
}