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

typedef struct {
  f64 x_max;
  f64 x_min;
  f64 y_max;
  f64 y_min;
  isize y_max_index;
  isize y_min_index;
  i32 x_base;
  i32 y_base;
  i32 h;
  i32 w;

} DrawInfo;

DftResult dft(Arena* a, f64Arr input);

f64Arr gen_wave(Arena* a, f64 freq, isize sample_freq_khz, isize samples);

f64Arr gen_wave_test(Arena* a);
f64 c_mag(Complex c);

f64Arr dft_to_plot_data(Arena* a, Sequence s);

typedef struct
{
  DrawInfo info;
} Plot;

typedef struct
{
  f64Arr data;
  void (*draw_elm)(int, int, DrawInfo);
} PlotData;

Plot pl_create_plot();


Plot pl_create_plot() 
{
  Plot p = { 0 };
  return p;
}


void pl_update_plot_info(Plot *plot, PlotData* plot_data)
{
  plot->info.y_max_index = -1;
  plot->info.y_min_index = -1;
  
  plot->info.x_min = 1000000000;
  plot->info.y_min = 1000000000;
  plot->info.x_max = -1000000000;
  plot->info.y_max = -1000000000;

  for (isize i = 0; i < plot_data->data.count; i++)
  {
    double m = plot_data->data.data[i];

    if (m > plot->info.y_max)
    {
      plot->info.y_max_index = i;
      plot->info.y_max = m;
    }

    if (m < plot->info.y_min)
    {
      plot->info.y_min_index = i;
      plot->info.y_min = m;
    }
    plot->info.x_max = max((double)i, plot->info.x_max);
    plot->info.x_min = min((double)i, plot->info.x_min);
  }
}

void pl_plot(Plot p, PlotData pd);

void pl_draw_dot_fn(int x, int y, DrawInfo info)
{
  DrawCircle(x, y, 2, RED);
}

void pl_draw_dft_fn(int x, int y, DrawInfo info)
{
  DrawLine(x, y, x, info.y_base + info.h, RED);
  DrawCircle(x, y, 2, RED);
}


f64 sample_rate = 300;
f32 wave_freq = 2.;
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

  f64Arr test = gen_wave_test(&perm_arena);
  DftResult dft_res = dft(&perm_arena, test);
  b32 draw_fft = false;

  
  Plot plot = { 0 };
  plot.info.x_base = 130;
  plot.info.y_base = 100;
  plot.info.h = 600;
  plot.info.w = 1000;

  PlotData plot_data = { 0 };

  plot_data.data = test;
  plot_data.draw_elm = &pl_draw_dot_fn;

  pl_update_plot_info(&plot, &plot_data);

  
  f64Arr dft_plot_data = { 0 };

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

    if (GuiButton((Rectangle) { 10, 10, 80, 40 }, "Reload Data"))
    {
      arena_reset(&perm_arena);
      test = gen_wave_test(&perm_arena);
      dft_res = dft(&perm_arena, test);
      dft_plot_data.count = 0;
      dft_plot_data.capacity = 0;
    }

    if (GuiButton((Rectangle) { 10, 100, 80, 40 }, "Swith"))
    {
      draw_fft = !draw_fft;
      if (draw_fft)
      {
        plot_data.data = dft_to_plot_data(&perm_arena, dft_res.dft_res);
        plot_data.draw_elm = &pl_draw_dft_fn;
        
      }
      else 
      {     
          plot_data.data = test; 
          plot_data.draw_elm = &pl_draw_dot_fn;      
      }

      pl_update_plot_info(&plot, &plot_data);
      
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);


    pl_plot(plot, plot_data);

    //f64 freq = dft_res.freq_bins.data[draw_info.y_max_index];
    //// Draw frequency bin with highest number
    //number_s = s8_f64_to_s8(&frame_arena, freq, 1);
    //s8 max_freq_s = s8_concat(&frame_arena, s8_from_literal("largests frequency bin = "), number_s);
    //s8_append_zero(&frame_arena, &max_freq_s);
    //DrawText(max_freq_s.data, 300, 20, 20, BLACK);




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

f64Arr dft_to_plot_data(Arena* a,Sequence s)
{
  // just get real data 
  f64Arr res = f64Arr_empty(a, s.count);

  res.count = s.count;
  for (isize i = 0; i < s.count; i++)
  {
    res.data[i] = c_mag(s.data[i]);
  }
  return res;
}

void pl_plot(Plot p, PlotData pd)
{

  DrawRectangle(p.info.x_base, p.info.y_base, p.info.w, p.info.h, WHITE);

  for (isize i = 0; i < pd.data.count; i++)
  {
    double m = pd.data.data[i];

    // range lerp
    // 0 - 1 range, inversed
    double x = (i - p.info.x_min) / (p.info.x_max - p.info.x_min);
    x = x * p.info.w;;

    // 0 - 1 range, inversed since screen space coordinate system i 0,0 at top left, and not bottom
    double y = 1.0 - (m - p.info.y_min) / (p.info.y_max - p.info.y_min);
    y = y * p.info.h;

    pd.draw_elm((int)x + p.info.x_base, (int)y + p.info.y_base, p.info);
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

// Only comutes the first half, since the rest is not use full when using real values, negative frequemcies
// for inverse we might need it again
DftResult dft(Arena* a, f64Arr input)
{
  DftResult res = { 0 };
  

  double N = (double)input.count;

  res.dft_res.capacity = input.capacity;
  res.dft_res.count = 0;
  res.dft_res.data = arena_alloc(a, Complex, input.count);
  
  // only compute the first N/2 values, since the rest are useless for realtime data
  for (isize k = 0; k < N/2 + 1; k++)
  {
    
    f64 real = 0.0;
    f64 imag = 0.0;
    for (isize i = 0; i < N; i++)
    {
      f64 x_n = input.data[i];
      double n = (double)i;

      double angle = 2 * PI * k * n / N;
      real += x_n * cos(angle);
      imag += -x_n * sin(angle);
    }

    Complex next = { 0 };
    next.r = real;
    next.i = imag;
    Sequence_add(a, &res.dft_res, next);
  }  


  f64Arr frequencies  = f64Arr_empty(a, (isize)N / 2 + 1);
  //f64Arr frequencies = f64Arr_empty(a, N);
  for (isize k = 0; k < frequencies.capacity; k++)
  {
    double freq = k * sample_rate / N;
    f64Arr_add(a, &frequencies, freq);
  }

  res.freq_bins = frequencies;

  return res;
}

f64Arr gen_wave_test(Arena* a)
{
  // sample 3 sec
  // wave is 1 hz
  // Sample 10 sec, with sample rate is number of samles
  isize samples = 3 * (isize)sample_rate;
  f64Arr res = f64Arr_empty(a, samples);

  // each sample is 1/samples of a sec
  f64 step = 2.0 * PI / sample_rate;
  
  for (isize i = 0; i < res.capacity; i++)
  {
    f64 v = sin(wave_freq * i * step);

    v += sin((wave_freq + 1) * i * step);
    v += sin((wave_freq + 100) * i * step + 30*step) * 0.5;

    f64Arr_add(a, &res, v);
  }

  return res;
}

f64Arr gen_wave(Arena* a, double freq, isize sample_freq_khz, isize samples)
{
  f64Arr res = f64Arr_empty(a, samples);

  

  return res;
}