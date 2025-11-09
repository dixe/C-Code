#pragma warning(push, 0)
#pragma warning (disable : 4996 6031 6387 6029 26451) // ignore fopen insecure in raylib
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma warning(pop)

#include "arena.h"
#include "hashmap.h"
#include "generated.h"
#include "types.h"

#define FOR(it, count) for(i32 it = 0; it < count; it++)

typedef struct 
{
  i32 activelimb;
  Segments next_state;
} Ikm;

typedef struct {
  Arena frame_arena;
  Arena perm_arena;
  Segments segments;  
  Vector2 target;

  i32 activelimb;
} Context;

Context ctx = { 0};
const i32 screenWidth = 1200;
const i32 screenHeight = 800;
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  ctx.frame_arena = arena_create(512);

  ctx.perm_arena = arena_create(512);

  InitWindow(screenWidth, screenHeight, "Robot arm");
  //--------------------------------------------------------------------------------------

  ctx.segments = Segments_empty(&ctx.perm_arena, 4);

  Segment a0 = { 0, 300 };
  Segment a1 = { 0.0, 150 };
  Segment a2 = { 0.0, 120 };

  Segments_add(&ctx.perm_arena, &ctx.segments, a0);
  Segments_add(&ctx.perm_arena, &ctx.segments, a1);
  Segments_add(&ctx.perm_arena, &ctx.segments, a2);


  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {   

    BeginDrawing();

    ClearBackground(RAYWHITE);

    VecArr points = VecArr_empty(&ctx.frame_arena, ctx.segments.count + 1);
    Vector2 base = { 200, 400 };
    VecArr_add(&ctx.frame_arena, &points, base);    
    f32 angle = 0.0;
    for (i32 i = 0; i < ctx.segments.count; i++)
    {
      Segment s = ctx.segments.data[i];
      Vector2 end = { 0 };
      angle += s.angle;
      end.x = base.x + cosf(angle) * s.length;
      end.y = base.y + sinf(angle) * s.length;
      base = end;
      VecArr_add(&ctx.frame_arena, &points, base);
      
    }

  


    // UI------------
    b32 mouse_captured = false;
    for (i32 i = 0; i < ctx.segments.count; i++)
    {       
      if (GuiSlider((Rectangle) { 20, (f32)i * 30, 100, 30 }, "-pi", "pi", & ctx.segments.data[i].angle, -PI, PI))
      {
        mouse_captured = 1;
      }
    }

    if(!mouse_captured && IsMouseButtonPressed(1))
    {
      ctx.target = GetMousePosition();
    }

    FOR(i, points.count)
    {
      if (i < points.count - 1)
      {
        DrawLineEx(points.data[i], points.data[i + 1], 4, BLACK);
      }

      DrawCircleV(points.data[i], 10, RED);
    }

    FOR(i, points.count)
    { 
      if (GuiButton((Rectangle) { 20+ (float)i * 25, 100, 20, 20 }, TextFormat("%d", i)))
      {
        ctx.activelimb = i;
      }

      if (ctx.activelimb == i)
      {
        DrawRectangleLinesEx((Rectangle){ 18 + (float)i*25 , 98, 24,24 }, 2, GREEN);

      }

    }

    for (i32 i = 0; i < points.count; i++)
    {

    }


    DrawCircleV(ctx.target, 10, GREEN);



    
   
    EndDrawing();
    //----------------------------------------------------------------------------------

    arena_reset(&ctx.frame_arena);
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
