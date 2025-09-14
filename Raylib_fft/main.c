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


typedef struct {
  Sequence dft_res;
  f64Arr freq_bins;
} DftResult;

DftResult dft(Arena* a, Sequence input);

Sequence gen_wave(Arena* a, f64 freq, isize sample_freq_khz, isize samples);

Sequence gen_wave_test(Arena* a);
f64 c_mag(Complex c);
void draw_sequence(Arena* a, Sequence s);

f64 sample_rate = 500;
f32 wave_freq = 0.;
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
  DftResult dft_res = dft(&perm_arena, test);
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

    if (GuiSlider ((Rectangle) { 100, 10, 120, 40 }, "1", "30", &wave_freq, 1, 30)){
      wave_freq = (f32)((int)wave_freq);
    }

    s8 number_s = s8_f64_to_s8(&frame_arena, (f64)wave_freq, 2);
    s8_append_zero(&frame_arena, &number_s);
    DrawText(number_s.data, 250, 20, 16, BLACK);

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
      draw_sequence(&frame_arena, dft_res.dft_res);
    }
    else {
      draw_sequence(&frame_arena, test);
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

void draw_sequence(Arena* frame_arena, Sequence s) 
{
 

  isize y_max_idx = -1;
  isize y_min_idx = -1;
  double x_min = 1000000000;
  double y_min = 1000000000;
  double x_max = -1000000000;
  double y_max = -1000000000;

  for (isize i = 0; i < s.count; i++)
  {
    double m = s.data[i].r; // c_mag(s.data[i]);
    if (m > 100)
    {
      isize debug = 2;
    }

    if (m > y_max)
    {
      y_max_idx = i;
      y_max = m;
    }

    if (m < y_min)
    {
      y_min_idx = i;
      y_min = m;
    }
    //y_max = max(m, y_max);
    
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
  i32 x_base = 130;
  i32 y_base = 100;
  i32 h = 600;
  i32 w = 1000; 
  
  DrawRectangle(x_base, y_base, w, h, WHITE);

  // draw line at y = 0;
  double y0 = 1.0 - (0 - y_min) / (y_max - y_min);

  // draw 0 at 0 line
  int y_0 = (int)(y0 * h + y_base);
  DrawText("0", 100, y_0 - 8, 16, BLACK);

  // draw max at top
  s8 number_s = s8_f64_to_s8(frame_arena, y_max, 2); 
  s8_append_zero(frame_arena, &number_s);
  DrawText(number_s.data, x_base- 20, y_base - 20, 16, BLACK);


  //draw min at bottom
  number_s = s8_f64_to_s8(frame_arena, y_min, 2);
  s8_append_zero(frame_arena, &number_s);
  DrawText(number_s.data, x_base - 20, y_base + h + 20, 16, BLACK);

  // Draw frequency bin with highest number
  number_s = s8_isize_to_s8(frame_arena, y_max_idx);
  s8 max_freq_s = s8_concat(frame_arena, s8_from_literal("largests frequency bin = "), number_s);
  s8_append_zero(frame_arena, &max_freq_s);
  DrawText(max_freq_s.data, 300, 20, 20, BLACK);



  DrawLine(x_base, y_0, x_base + w, y_0, BLACK);

  for (isize i = 0; i < s.count; i++)
  {
    double m = s.data[i].r; // c_mag(s.data[i]);
    //double m = c_mag(s.data[i]);
    // map x into range 0-1000
    
    // range lerp
    // 0 - 1 range, inversed
    double x =  (i - x_min) / (x_max - x_min);
    x = x * w;;

    // 0 - 1 range, inversed since screen space coordinate system i 0,0 at top left, and not bottom
    double y = 1.0 - (m - y_min) / (y_max - y_min);
    y = y * h;


    DrawCircle((int)x + x_base, (int)y + y_base, 2, RED);

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


DftResult dft(Arena* a, Sequence input)
{
  DftResult res = { 0 };
  

  double N = (double)input.count;

  res.dft_res.capacity = input.capacity;
  res.dft_res.count = 0;
  res.dft_res.data = arena_alloc(a, Complex, input.count);
  
  for (isize k = 0; k < N; k++)
  {
    
    f64 real = 0.0;
    f64 imag = 0.0;
    for (isize i = 0; i < N; i++)
    {
      f64 x_n = input.data[i].r;
      double n = (double)i;

      double angle = 2 * PI * k * n / N;
      real += x_n * cos(angle);
      imag -= x_n * sin(angle);
    }

    Complex next = { 0 };
    next.r = real;
    next.i = imag;
    Sequence_add(a, &res.dft_res, next);
  }  


  f64Arr frequencies  = f64Arr_empty(a, (isize)N / 2 + 1);
  for (isize k = 0; k < frequencies.capacity; k++)
  {
    double freq = k * sample_rate / N;
    f64Arr_add(a, &frequencies, freq);
  }

  res.freq_bins = frequencies;

  return res;
}


Sequence gen_wave_test(Arena* a)
{

  // sample 3 sec
  // wave is 1 hz
  // Sample 10 sec, with sample rate is number of samles
  isize samples = 3 * (isize)sample_rate;
  Sequence res = Sequence_empty(a, samples);

  // each sample is 1/samples of a sec
  f64 step = 2.0 * PI / sample_rate;
  
  for (isize i = 0; i < res.capacity; i++)
  {
    Complex c = { 0 };
    c.r = sin(wave_freq * i * step) + 1;   

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