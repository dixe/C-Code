#pragma warning(push, 0)
#pragma warning (disable : 4996 6031 6387 6029 26451) // ignore fopen insecure in raylib
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma warning(pop)

#include "arena.h"
#include "hashmap.h"
#include "types.h"
#include "generated.h"

Sequence dft(Arena* a, Sequence input);

Sequence gen_wave(Arena* a, double freq, isize sample_freq_khz, isize samples);

Sequence gen_wave_test(Arena* a);
double c_mag(Complex c);
void draw_sequence(Sequence s);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------

  const i32 screenWidth = 1200;
  const i32 screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "Fourier");
  //--------------------------------------------------------------------------------------

  
  Arena frame_arena = arena_create(512);

  Arena perm_arena = arena_create(512);

  Sequence test = gen_wave_test(&perm_arena);
  Sequence dft_res = dft(&perm_arena, test);
  b32 draw_fft = false;

  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {
    Sequence s = Sequence_empty(&frame_arena, 4);

    Complex c0 = { 1,0 };
    Complex c1 = { 2, -1 };
    Complex c2 = { 0, -1 };
    Complex c3 = { -1, 2 };

    Sequence_add(&frame_arena, &s, c0);
    Sequence_add(&frame_arena, &s, c1);
    Sequence_add(&frame_arena, &s, c2);
    Sequence_add(&frame_arena, &s, c3);


    

    if (GuiButton((Rectangle) { 10, 10, 80, 40 }, "Reload test Data"))
    {
      arena_reset(&perm_arena);
      test = gen_wave_test(&perm_arena);
      dft_res = dft(&perm_arena, test);
    }

    if (GuiButton((Rectangle) { 10, 100, 80, 40 }, "Swith"))
    {
      draw_fft = !draw_fft;
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (draw_fft)
    {
      draw_sequence(dft_res);
    }
    else {
      draw_sequence(test);
    }
    
     

    EndDrawing();
    arena_reset(&frame_arena);
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

//--------------------------------------------------------------------------------------
// Module functions definition
//--------------------------------------------------------------------------------------
//
//double min(double a, double b)
//{
//  return a < b ? a : b;
//}

//double max(double a, double b)
//{
//  return a > b ? a : b;
//}

void draw_sequence(Sequence s) 
{

  DrawRectangle(100, 100, 1000, 600, BLACK);

  double x_min = 1000000000;
  double y_min = 1000000000;
  double x_max = -1000000000;
  double y_max = -1000000000;

  for (isize i = 0; i < s.count; i++)
  {
    double m = s.data[i].r; // c_mag(s.data[i]);
    if (m > 400)
    {
      isize debug = 2;
    }

    y_max = max(m, y_max);
    y_min = min(m, y_min);

    x_max = max(i, x_max);

    x_min = min(i, x_min);

    //y_min = min(m, y_min);    
    //x_min = min(v.i, x_min);

    //y_max = max(v.r, y_max);
    //x_max = max(v.i, x_max);
    //x_max = max(i, x_max);

  }


  // draw in rectangle x= 100, y=100, w=1000, h=600

  for (isize i = 0; i < s.count; i++)
  {
    double m = s.data[i].r; // c_mag(s.data[i]);
    //double m = c_mag(s.data[i]);
    // map x into range 0-1000
    
    // range lerp
    // 0 - 1 range, inversed
    double x =  (i - x_min) / (x_max - x_min);
    x = x * 1000.0;

    // 0 - 1 range, inversed since screen space coordinate system i 0,0 at top left, and not bottom
    double y = 1.0 - (m - y_min) / (y_max - y_min);
    y = y * 600;


    DrawCircle((int)x + 100, (int)y + 100, 2, RED);





  }


}

Complex c_mul(Complex c1, Complex c2)
{
  Complex res = { 0 };
  double a = c1.r;
  double b = c1.i;
  double c = c2.r;
  double d = c2.i;

  res.r = a * c - b * d;
  res.i = a * d + b * c;

  return res;
}

double c_mag(Complex c)
{
  return sqrt(c.r * c.r + c.i * c.i);
}

Complex c_add(Complex a, Complex b)
{
  Complex c = { a.r + b.r, a.i + b.i };
  return c;
     
}


Sequence dft(Arena* a, Sequence input) 
{
  Sequence res = { 0 };

  double N = (double)input.count;

  res.capacity = input.capacity;
  res.count = 0;
  res.data = arena_alloc(a, Complex, input.count);

  
  for (isize k = 0; k < N; k++)
  {
    Complex next = { 0 };
    for (isize i = 0; i < N; i++)
    {
      double n = (double)i;

      Complex c = { 0 };
      double exp = -2 * PI * (k / N) * n;
      c.r = cos(exp);
      c.i = sin(exp);
      Complex v = input.data[i];
      v.i = v.i / 160.0;
      Complex prod = c_mul(v, c);
      next = c_add(next, prod);
    }

    Sequence_add(a, &res, next);
  }  

  double sample_rate = 1000;



  //frequencies = [];
  //for (isize k = 0; k < N / 2 + 1; k++)
  //{
  //  double freq = k * sample_rate / N;
  //  frequencies.append(freq)
  //}

  //print(frequencies)

  return res;
}


Sequence gen_wave_test(Arena* a)
{
  Sequence res = Sequence_empty(a, 1000);

  for (isize i = 0; i < res.capacity; i++)
  {
    Complex c = { 0 };

    c.r = sin((double)i/ 100.0);
    //c.i = (double)i;

    Sequence_add(a, &res, c);
  }

  return res;
}



Sequence gen_wave(Arena* a, double freq, isize sample_freq_khz, isize samples)
{
  Sequence res = Sequence_empty(a, samples);

  double x = 1.0 / ((double)sample_freq_khz * 1000.0) * freq;
  for (isize i = 0; i < samples; i++)
  {
    Complex c = { 0 };

    c.r = sin((double)i * freq);
    c.i = (double)i;

    Sequence_add(a, &res, c);
  }

  return res;
}