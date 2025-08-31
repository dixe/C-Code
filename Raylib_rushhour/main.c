#pragma warning(push, 0)
#pragma warning (disable : 4996 6031 6387 6029 26451) // ignore fopen insecure in raylib
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma warning(pop)

#include "arena.h"
#include "hashmap.h"
#include "dyn_array.h"
#include "s_string.h"
#include "io.h"
#include "types.h"
#include "containers.h"



const i32 screenWidth = 1200;
const i32 screenHeight = 800;
i32 tileW = 80;
i32 brick_padding = 3;




i32 anchor_width = 14;
b32 MoveGrid(Vector2 mouse, b32 resizing);
MovingBrick MoveBrick(Vector2 mouse, GameState* bricks, MovingBrick moving_brick);
void DrawBoard(GameState game, MovingBrick moving_brick);
void AddBrick(GameState* game, i32 left_col, i32 top_row, i32 len, DIRECTION dir, Color color);
void UpdateBoard(GameState* game);
b32 IsWinningState(Bricks bricks);
Rectangle BrickRect(Brick b);

Rectangle TileRect(i32 col, i32 row, i32 horizontal_tile, i32 vertical_tiles);
Moves FindMoves(Arena* a, GameState game);
void UpdateGameToBoard(GameState* game, BoardState board);

BoardGraph* FindAllMoves(Arena* data_arena, Arena* scratch, GameState game);

b32 IsMoveValid(GameState* game, Brick b, i32 new_row, i32 new_col);
Vector2 base_anchor;
void ResetBricks(GameState* game) { 
  
  memset(&game->board, 0, sizeof(game->board));
  memset(&game->bricks, 0, sizeof(game->bricks));

  // TARGET PIECE
  AddBrick(game, 1, 2, 2, HORIZONTAL, GREEN);
  AddBrick(game, 4, 2, 2, VERTICAL, BLUE);
  AddBrick(game, 4, 4, 2, VERTICAL, BLUE);

  AddBrick(game, 1, 1, 4, HORIZONTAL, GRAY);
  AddBrick(game, 0, 1, 2, VERTICAL, YELLOW);
}


Rectangle rect(i32 x, i32 y, i32 w, i32 h)
{
  return (Rectangle) { (f32)x, (f32)y, (f32)w, (f32)h };
}
s8 button_title(Arena *arena, Move move) {

  s8 res = s8_empty(arena, 8); 
  res.data[0] = move.brick_id + 48;
  res.byte_len += 1;
  if (move.horizontal == 1)
  {
    s8_append(arena, &res, s8_from_literal(" Right"), 0, 6);
  }
  else if (move.horizontal == -1)
  {
    s8_append(arena, &res, s8_from_literal(" Left"), 0, 5);
  }
  else if (move.vertical == 1)
  {
    s8_append(arena, &res, s8_from_literal(" Down"), 0, 5);
  }
  else if (move.vertical == -1)
  {
    s8_append(arena, &res, s8_from_literal(" Up"), 0, 3);
  }
  res.data[res.byte_len] = 0;
  return res;
}

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
  Arena frame_arena = arena_create(1024);
  
  GameState game = { 0 };
  
  ResetBricks(&game);
  // Main game loop
  Moves available_moves = { 0 };
  HashMapTrie* all_moves = { 0 };
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {
    Vector2 mouse = GetMousePosition();

    moving_grid = MoveGrid(mouse, moving_grid);
    moving_brick = MoveBrick(mouse, &game, moving_brick);


    if (GuiButton((Rectangle) { 100, 10, 80, 40 }, "Reset bricks"))
    {
      ResetBricks(&game);
    } 
    
    if (GuiButton((Rectangle) { 100, 55, 80, 40 }, "Find all moves"))
    {
      all_moves = FindAllMoves(&game_arena, &frame_arena, game);

    }

    if (GuiButton((Rectangle) { 10, 10, 80, 40 }, "Find moves"))
    { 
      //available_moves = FindAllMoves(&game_arena, &frame_arena, game);
      available_moves = FindMoves(&game_arena, game);      
    }

    s8 key = s8_from_bytes((u8*)&game.board, sizeof(BoardState));
    Moves* moves = (Moves*) hmt_get(&all_moves, key);

    if (moves != 0)
    {
      for (i32 i = 0; i < moves->count; i++)
      {
        s8 text = button_title(&frame_arena, moves->data[i]);
        if (GuiButton(rect(10, 55 + 5 * i + 40 * i, 80, 40), text.data))
        {
          UpdateGameToBoard(&game, moves->data[i].board);
        }
      }
    }
    else {
      for (i32 i = 0; i < available_moves.count; i++)
      {

        if (GuiButton(rect(10, 55 + 5 + 40 * i, 80, 40 ), "Show move"))
        {
          UpdateGameToBoard(&game, available_moves.data[i].board);
        }
      }
    }

  

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(BLACK);
    DrawBoard(game, moving_brick);

    if (IsWinningState(game.bricks))
    {
      DrawText("GREAT SUCCESS", 400, 40, 30, GREEN);
    }

    EndDrawing();

    arena_reset(&frame_arena);

  }

  CloseWindow();
  return 0;
}

void hm_print(s8 key, void* val)
{
  for (i32 i = 0; i < key.byte_len; i++)
  {
    key.data[i] += 48;
  }
  s8_println(key);
  for (i32 i = 0; i < key.byte_len; i++)
  {
    key.data[i] -= 48;
  }

}

BoardGraph* FindAllMoves(Arena* data_arena, Arena* scratch, GameState game)
{
  HashMapTrie* res = { 0 };

  i32 count = 0;
  i32 count2 = 0;
  HashMapTrie *seen_states = { 0 };    
  Moves unchecked = { 0 };


  GameState tmpGame = { 0 };
  for (i32 i = 0; i < game.bricks.count; i++)
  {
    Brick b = game.bricks.data[i];
    AddBrick(&tmpGame, b.left_col, b.top_row, b.len, b.dir, b.color);
  }

  // add current game state as first unchecked state
  Move m = { 0 };
  m.board = game.board;
  moves_add(scratch, &unchecked, &m);

  // add current game state as seen, so we don't readd it to unchecked queue
  s8 start_key = s8_from_bytes((u8*)&game.board, sizeof(BoardState));
  hmt_insert_get(&seen_states, start_key, i32, scratch);

  while (unchecked.count > 0)
  {
    // pop next move
    Move next = unchecked.data[unchecked.count - 1];
   
    if (next.board.data[15] == 1 &&
      next.board.data[14] == 1 &&
      next.board.data[16] == 2 &&
      next.board.data[22] == 2)
    {
      i32 debug = 2;
    }

    unchecked.count--;
    s8 next_key = s8_from_bytes((u8*)&next.board, sizeof(BoardState));
    Moves* moves = hmt_insert_get(&res, next_key, Moves, data_arena);
    count += 1;

    UpdateGameToBoard(&tmpGame, next.board);
    Moves available_moves = FindMoves(scratch, tmpGame);
    for (i32 i = 0; i < available_moves.count; i++)
    {
      Move neighbour = available_moves.data[i];     
      moves_add(data_arena, moves, &neighbour);      
      
      s8 neighbour_key = s8_from_bytes((u8*)&neighbour.board, sizeof(BoardState));
      
      if (!hmt_contains(&seen_states, neighbour_key))
      { 
       
        if (neighbour.board.data[15] == 1 &&
          neighbour.board.data[14] == 1 &&
          neighbour.board.data[16] == 2 &&
          neighbour.board.data[22] == 2)
        {
          i32 debug = 2;
        }
        moves_add(scratch, &unchecked, &neighbour);
        hmt_insert_get(&seen_states, neighbour_key, i32, scratch);
      }
    }
  }

  return res;

}

Moves FindMoves(Arena* a, GameState game)
{
  Moves res = { 0 };

  for (i32 i = 0; i < game.bricks.count; i++)
  {    
    // move brick 1 step along dir, in both direction;
    Brick b = game.bricks.data[i];
    for (i32 j = -1; j < 2; j += 2)
    {
      // only the dir
      int new_row = (VERTICAL == b.dir) * j + b.top_row;
      int new_col = (HORIZONTAL == b.dir) * j + b.left_col;
      if (IsMoveValid(&game, b, new_row, new_col))
      {
        GameState new_move_state = { 0 };
        // set new_move_state to old state
        memcpy(&new_move_state, &game, sizeof(GameState));        
       
        new_move_state.bricks.data[i].top_row = new_row;;
        new_move_state.bricks.data[i].left_col = new_col;
        UpdateBoard(&new_move_state);       

        if (res.count >= res.capacity)
        {
          // realloc
        }
        Move m = { 0 };
        m.board = new_move_state.board;
        m.brick_id = b.brick_id;
        m.horizontal = new_col - b.left_col;
        m.vertical = new_row - b.top_row;
        moves_add(a, &res, &m);
      }
    }
  }

  return res;
}

void UpdateGameToBoard(GameState* game, BoardState board)
{
  u8 seen_bricks[GRIDW * GRIDW] = { 0 };

  game->board = board;
  for (i32 i = 0; i < GRIDW; i++)
  {
    for (i32 j = 0; j < GRIDW; j++)
    {
      u8 id = game->board.data[i * GRIDW + j];
      if (id !=0 && seen_bricks[id] == 0)
      {
        game->bricks.data[id - 1].top_row = i;
        game->bricks.data[id - 1].left_col = j;        
        seen_bricks[id] = 1;
      }
    }
  }
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
  brick->brick_id = game->bricks.count + 1;
  brick->color = color;
  brick->left_col = left_col;
  brick->top_row = top_row;
  brick->len = len;
  brick->dir = dir;
  game->bricks.count += 1;

  UpdateBoard(game);
}



b32 IsMoveValid(GameState* game, Brick b, i32 new_row, i32 new_col) {
  b32 valid = 1;
  if (new_row < 0 || new_row >= GRIDW || new_col < 0 || new_col >= GRIDW)
  {
    return 0;
  }

  for (i32 j = 0; j < b.len; j++)
  {
    i32 row = new_row + (VERTICAL == b.dir) * j;
    i32 col = new_col + (HORIZONTAL == b.dir) * j;
    u8 tile_id = game->board.data[row * GRIDW + col];
    valid &= (row < GRIDW&& row >= 0 && col >= 0 && col < GRIDW) && tile_id == 0 || tile_id == b.brick_id;
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
          i32 mouse_start_col = (i32)(moving_brick.starting_pos.x - base_anchor.x) / tileW;
          i32 mouse_end_col= (i32)(moving_brick.current_pos.x - base_anchor.x) / tileW;
          i32 mouse_start_row = (i32)(moving_brick.starting_pos.y - base_anchor.y) / tileW;
          i32 mouse_end_row = (i32)(moving_brick.current_pos.y - base_anchor.y) / tileW;

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
    Rectangle anchor = rect((i32)base_anchor.x - anchor_width, (i32)base_anchor.y - anchor_width, anchor_width, anchor_width );
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, anchor)) resizing = true;
  }
  return resizing;
}

void DrawBoard(GameState game, MovingBrick moving_brick)
{

  // draw base_anchor

  DrawRectangle((i32)base_anchor.x - anchor_width, (i32)base_anchor.y - anchor_width, anchor_width, anchor_width, MAROON);

  // draw grid
  i32 baseX = (i32)base_anchor.x;
  i32 baseY = (i32)base_anchor.y;
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
    Rectangle br = BrickRect(game.bricks.data[i]);

    i32 x_offset = 0;
    i32 y_offset = 0;
    if (moving_brick.brick_id == game.bricks.data[i].brick_id)
    {
      x_offset = (game.bricks.data[i].dir == HORIZONTAL) * (i32)(moving_brick.current_pos.x - moving_brick.starting_pos.x);
      y_offset = (game.bricks.data[i].dir == VERTICAL) * (i32)(moving_brick.current_pos.y - moving_brick.starting_pos.y);
    }

    DrawRectangle((i32)br.x + x_offset, (i32)br.y + y_offset, (i32)br.width, (i32)br.height, game.bricks.data[i].color);
  }

  i32 idx = 0;
  for (i32 i = 0; i < GRIDW; i++)
  {
    for (i32 j = 0; j < GRIDW; j++)
    {
      char str[6] = { 0 };
      u8 id = game.board.data[i * GRIDW + j];
      snprintf(str, 6, "%d", id > 0 ? id : idx);
      Rectangle tile_rect = TileRect(i, j, 1, 1);
      Color color = id == 0 ? WHITE : BLACK;
      i32 x = (i32)(tile_rect.x + tile_rect.width / 2 - 5);
      i32 y = (i32)(tile_rect.y + tile_rect.height / 2 - 5);
      DrawText(str, x, y, 20, color);
      idx += 1;
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
  i32 base_x = (i32)base_anchor.x;
  i32 base_y = (i32)base_anchor.y;
  i32 x = base_x + col * tileW + brick_padding;
  i32 y = base_y + row * tileW + brick_padding;
  i32 width = horizontal_tile * tileW; 
  i32 height = vertical_tiles * tileW; 

  return rect(x, y, width - brick_padding * 2, height - brick_padding * 2 );
}

Rectangle BrickRect(Brick b) {

  return TileRect(b.top_row, b.left_col, 1 + (b.dir == HORIZONTAL) * (b.len - 1), 1 + (b.dir == VERTICAL) * (b.len - 1));
}