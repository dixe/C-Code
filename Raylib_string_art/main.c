#include "raylib.h"
#include "arena.h"
#include "hashmap.h"
#pragma warning (disable : 4996) // ignore fopen insecure in raylib
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

  // See https://www.youtube.com/watch?v=dBlSmg5T13M
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "raylib [text] example - draw text inside a rectangle");

  bool resizing = false;

  Rectangle container = { 25.0f, 25.0f, screenWidth - 50.0f, screenHeight - 250.0f };
  Rectangle resizer = { container.x + container.width - 17, container.y + container.height - 17, 14, 14 };

  // Minimum width and heigh for the container rectangle
  const float minWidth = 60;
  const float minHeight = 60;
  const float maxWidth = screenWidth - 50.0f;
  const float maxHeight = screenHeight - 160.0f;

  Vector2 lastMouse = { 0.0f, 0.0f }; // Stores last mouse coordinates
  Color borderColor = RAYWHITE;         // Container border color

  Arena frame_arena = arena_create(1024*1024);

  SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  float radius = 300.0f;
  float nails = 30;

  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
  
    Vector2 mouse = GetMousePosition();
    lastMouse = mouse; // Update mouse
    //----------------------------------------------------------------------------------

    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(CLITERAL(Color) { 100, 100, 100, 100 });
    DrawCircle(screenWidth/2, screenHeight/2, radius, WHITE);

    i32 sliderRes = GuiSlider((Rectangle) { 24, 24, 120, 30 }, "2", "600", &nails, 2.0f, 600.0f);

    // nail start at top
    f32 nail_angle_rad = 2.0f * PI / nails;
    for (f32 angle = 0; angle < 2.0f * PI; angle += nail_angle_rad)
    {      
      f32 x = screenWidth / 2.0f + sinf(angle) * radius;
      f32 y = screenWidth / 2.0f + cosf(angle) * radius;
      DrawCircle((i32)x, (i32)y, 3.0f, BLACK);

      DrawLine(x, y, screenWidth / 2.0f + radius, screenHeight / 2.0f, BLACK);
    }

    EndDrawing();
  }


  CloseWindow();
  return 0;
}