#include "raylib.h"
#include "arena.h"
#include "hashmap.h"
#pragma warning (disable : 4996) // ignore fopen insecure in raylib
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// min x  || Ax - b||^2 st x=0 or x=1
// b is the target image, in a single long array (vec) size radius * radius
// A matrix where each columns is each line as a single array (vec) also size radius * radius
// x is vector of length nails, where each entry is either 0 or 1.

// algorithm i to have the current string art image. 
// Try each new line, starting from current nail. 
// See which line reduces ||newImage - originalImage||^2 the most
// pick that line, keep going until we don't improve on the error


// model each line
//

// 
typedef struct {
  float* data;
  isize count;
} Imagemat;

Vector2 nail_to_screen_point(i32 nail);
Imagemat create_line_image(Arena* a, i32 nail1, i32 nail2);
f32 pixel_line_distance(Vector2 p1, Vector2 p2, Vector2 pixel_pos);


f32 nails = 30; 
const i32 screenWidth = 800;
const i32 screenHeight = 800;
f32 radius = 300.0f;
Texture2D line_texture;


void calc_line(Arena* line_arena, Image line_image) {
  Imagemat line1 = create_line_image(line_arena, 0, 1);
  i32 a = 2;
  line_image.data = line1.data;
  line_texture = LoadTextureFromImage(line_image);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

  // See https://www.youtube.com/watch?v=dBlSmg5T13M
  // Initialization
  //--------------------------------------------------------------------------------------
  

  InitWindow(screenWidth, screenHeight, "raylib [text] example - draw text inside a rectangle");

  bool resizing = false;

  Rectangle container = { 25.0f, 25.0f, screenWidth - 50.0f, screenHeight - 250.0f };
  Rectangle resizer = { container.x + container.width - 17, container.y + container.height - 17, 14, 14 };

  Arena frame_arena = arena_create(1024*1024);

  Arena line_arena = arena_create(1024 * 1024);

  SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------



  // load image
  Image image = LoadImage("eye.jpeg");     // Loaded in CPU memory (RAM)
  ImageColorGrayscale(&image);
  ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R32); // 1 f32 float;
  f32 ratio = image.width * 1.0f / image.height;
  ImageResize(&image, radius * 2 , radius* 2 );

  Texture2D texture = LoadTextureFromImage(image);          // Image converted to texture, GPU memory (VRAM)

  Image line_image = ImageCopy(image);
  ImageFormat(&line_image, PIXELFORMAT_UNCOMPRESSED_R32); // 1 f32 float per pixel
  calc_line(&line_arena, line_image);
  
  
  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(CLITERAL(Color) { 100, 100, 100, 100 });


    DrawTexture(line_texture, screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2, WHITE);

    // DrawCircle(screenWidth/2, screenHeight/2, radius, WHITE);

    i32 sliderRes = GuiSlider((Rectangle) { 24, 24, 120, 30 }, "2", "600", &nails, 2.0f, 600.0f);
    if (GuiButton((Rectangle) { 24, 54, 120, 30 }, "Recalc line"))
    {
      calc_line(&line_arena, line_image);
    }


    for (i32 i = 0; i < nails; i++)
    {
      Vector2 nail_point = nail_to_screen_point(i);
      DrawCircle(nail_point.x, nail_point.y, 3.0f, BLACK);

      //DrawLine(nail_point.x, nail_point.y, screenWidth / 2.0f + radius, screenHeight / 2.0f, BLACK);   
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

Vector2 nail_to_screen_point(i32 nail) {

  // nail start at top
  f32 nail_angle_rad = 2.0f * PI / nails;
  f32 nail_angle = nail_angle_rad * (f32)nail;

  Vector2 res;
  res.x = screenWidth / 2.0f + sinf(nail_angle) * radius;
  res.y = screenHeight / 2.0f + cosf(nail_angle) * radius;
  return res;
}

Vector2 nail_to_image_point(i32 nail) {

  // nail start at top
  f32 nail_angle_rad = 2.0f * PI / nails;
  f32 nail_angle = nail_angle_rad* (f32)nail;

  Vector2 res;
  f32 s = sinf(nail_angle);
  f32 c = cosf(nail_angle);
  res.x = radius + sinf(nail_angle) * radius;
  // flip y so coordinate system matches
  res.y = radius * 2.0f - (radius + cosf(nail_angle) * radius);
  return res;
}

f32 pixel_line_distance(Vector2 p1, Vector2 p2, Vector2 pixel_pos) {

  // from https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
  f32 numerator = abs((p2.y - p1.y) * pixel_pos.x - (p2.x - p1.x) * pixel_pos.y + p2.x * p1.y - p2.y * p1.x);
  f32 denominator = sqrtf((p2.y - p1.y) * (p2.y - p1.y) + (p2.x-p1.x) * (p2.x - p1.x));
  return numerator / denominator;
}

Imagemat create_line_image(Arena* a, i32 nail1, i32 nail2)
{
  // allocate
  Imagemat res = { 0 };
  res.count = 600 * 600;
  res.data = arena_alloc(a, f32, res.count);
  // set all to 0
  memset(res.data, 0, res.count);

  Vector2 nail1_pos = nail_to_image_point(nail1);
  Vector2 nail2_pos = nail_to_image_point(nail2);

  // we don;t have to go thougt all pixels, but only an extended bounding box of the two nail positions
  // TODO THAT

  float minV = 1000.0f;
  
  
  for (i32 i = 0; i < 600; i++) 
  {
    for (i32 j = 0; j < 600; j++)
    {      
      i32 index = j * 600 + i;
      res.data[index] = 0.0;
      // find pixel distance to line
      Vector2 pixel_pos = { 0 };
      pixel_pos.x = i;
      pixel_pos.y = j;
      if (i == 300)
      {
        i32 debug = 2;
      }
      f32 dist = pixel_line_distance(nail1_pos, nail2_pos, pixel_pos);
      
      minV = min(minV, dist);
      if (dist < 1.0)
      {
        res.data[index] = 1.0f - dist;
      }
      f32 d = dist;
    }
  }

  return res;

}