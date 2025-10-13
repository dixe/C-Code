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
#include "plot.h"

typedef struct {
  Sequence seq;
  PeakFreqArray peak_frequencies;
} DftResult;

typedef struct {
  Arena frame_arena;
  Arena perm_arena;
  f64Arr test;
  DftResult dft_res;
  b32 draw_fft;
  b32 use_dft;
  Plot plot;
  PlotData plot_data;
  f64Arr dft_plot_data;
} Context;


DftResult dft(Arena* a, f64Arr input);
f64Arr dft_inv(Arena* a, Sequence input);
DftResult fft(Arena* perm_arena, Arena* tmp_arena, f64Arr input);
DftResult transform(f64Arr input);


void calc_peaks(Arena* a, DftResult* res);
f64Arr gen_wave(Arena* a, f64 freq, isize sample_freq_khz, isize samples);

f64Arr gen_wave_test(Arena* a);
f64 c_mag(Complex c);

f64Arr dft_to_plot_data(Arena* a, Sequence s);

void remove_freq(DftResult* dft_res, PeakFreq peak_f);

void initialize_context();
void update_plot_data();

void set_plot_data(PlotData *p, f64Arr data)
{
  p->count = data.count;
  p->data = data.data;
}

f64 sample_rate = 2000;
f32 wave_freq = 1.0; 

Sequence fft_c_t(f64Arr input, Arena* tmp_arena, isize N);

Context ctx = { 0 };

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

  initialize_context();

  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {
    Sequence s = Sequence_empty(&ctx.frame_arena, 4);

    Complex c0 = { 1,0 };
    Complex c1 = { 2, -1 };
    Complex c2 = { 0, -1 };
    Complex c3 = { -1, 2 };

    Sequence_add(&ctx.frame_arena, &s, c0);
    Sequence_add(&ctx.frame_arena, &s, c1);
    Sequence_add(&ctx.frame_arena, &s, c2);
    Sequence_add(&ctx.frame_arena, &s, c3);


    f32 next_button_y = 10;
    f32 button_y_inc = 50;

    ///// UI CODE /////////
    if (GuiSlider ((Rectangle) { 100, 10, 120, 40 }, "1", "30", &wave_freq, 1, 30)){
      wave_freq = (f32)((int)wave_freq);
    }

    s8 number_s = s8_f64_to_s8(&ctx.frame_arena, (f64)wave_freq, 2);
    s8_append_zero(&ctx.frame_arena, &number_s);
    DrawText(number_s.data, 250, 20, 16, BLACK);

    if (GuiButton((Rectangle) { 10, next_button_y, 80, 40 }, "Reload Data"))
    {
      arena_reset(&ctx.perm_arena);
      ctx.test = gen_wave_test(&ctx.perm_arena);
      ctx.dft_res = transform(ctx.test);      
      ctx.dft_plot_data.count = 0;
      ctx.dft_plot_data.capacity = 0;
    }
    next_button_y += button_y_inc;

    if (GuiButton((Rectangle) { 10, next_button_y, 80, 40 }, "Inverse_dft"))
    {     
      ctx.test = dft_inv(&ctx.perm_arena, ctx.dft_res.seq);
      update_plot_data();
    }

    next_button_y += button_y_inc;

    if (GuiButton((Rectangle) { 10, next_button_y, 80, 40 }, "Swith View"))
    {
      ctx.draw_fft = !ctx.draw_fft;
      update_plot_data();
    }


    next_button_y += button_y_inc;

    if (GuiButton((Rectangle) { 10, next_button_y, 80, 40 }, "Switch algo"))
    {
      ctx.use_dft = !ctx.use_dft;
    }

    next_button_y += button_y_inc;

   
    if (GuiButton((Rectangle) { 10, next_button_y, 80, 40 }, "Run dft"))
    {
      ctx.dft_res = transform(ctx.test);
      ctx.dft_plot_data.count = 0;
      ctx.dft_plot_data.capacity = 0;
    }

    next_button_y += button_y_inc;

    BeginDrawing();

    ClearBackground(RAYWHITE);

    pl_plot(&ctx.frame_arena, ctx.plot, ctx.plot_data);

    if (ctx.dft_res.peak_frequencies.count > 0)
    {
      number_s = s8_isize_to_s8(&ctx.frame_arena, ctx.dft_res.peak_frequencies.count);
      s8 freq_s = s8_concat(&ctx.frame_arena, s8_from_literal("Peak frequencies ("), number_s);
      s8_append_c_str(&ctx.frame_arena, &freq_s, ") ");

      for (isize i = 0; i < ctx.dft_res.peak_frequencies.count; i++)
      {
        number_s = s8_f64_to_s8(&ctx.frame_arena, ctx.dft_res.peak_frequencies.data[i].freq, 1);
        s8_append(&ctx.frame_arena, &freq_s, number_s);
        if (i < ctx.dft_res.peak_frequencies.count - 1)
        {
          s8_append_c_str(&ctx.frame_arena, &freq_s, " ,");
        }

        s8 button_text = s8_concat(&ctx.frame_arena, s8_from_literal("Remove F: "), number_s);
        s8_append_zero(&ctx.frame_arena, &button_text);

        if (GuiButton((Rectangle) { (f32)10, next_button_y, (f32)80, (f32)40 }, button_text.data))
        {
          remove_freq(&ctx.dft_res, ctx.dft_res.peak_frequencies.data[i]);
        }
        next_button_y += button_y_inc;
      }

      s8_append_zero(&ctx.frame_arena, &freq_s);
      DrawText(freq_s.data, 300, 20, 20, BLACK);
    }




    EndDrawing();
    arena_reset(&ctx.frame_arena);
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

void initialize_context()
{
  ctx.frame_arena = arena_create(512);

  ctx.perm_arena = arena_create(512);

  ctx.test = gen_wave_test(&ctx.perm_arena);
  ctx.dft_res = transform(ctx.test);
  ctx.draw_fft = false;


  Plot plot = { 0 };
  plot.info.x_base = 130;
  plot.info.y_base = 100;
  plot.info.h = 600;
  plot.info.w = 1000;
 

  PlotData plot_data = { 0 };
  plot_data.color = RED;
  set_plot_data(&plot_data, ctx.test);
  plot_data.draw_elm = &pl_draw_dot_fn;
  pl_update_plot_info(&plot, &plot_data);
  
  ctx.plot = plot;
  ctx.plot_data = plot_data;
}


void update_plot_data()
{

  if (ctx.draw_fft)
  {
    set_plot_data(&ctx.plot_data, dft_to_plot_data(&ctx.perm_arena, ctx.dft_res.seq));
    ctx.plot_data.draw_elm = &pl_draw_dft_fn;
  }
  else
  {
    set_plot_data(&ctx.plot_data, ctx.test);
    ctx.plot_data.draw_elm = &pl_draw_dot_fn;
  }

  pl_update_plot_info(&ctx.plot, &ctx.plot_data);
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

Complex c_sub(Complex a, Complex b)
{
  Complex c = { a.r - b.r, a.i - b.i };
  return c;

}
DftResult transform(f64Arr input)
{
  if (ctx.use_dft)
  {
    return dft(&ctx.perm_arena, input);
  }

  return fft(&ctx.perm_arena, &ctx.frame_arena, input);

}

// Only comutes the first half, since the rest is not use full when using real values, negative frequemcies
// for inverse we might need it again
DftResult dft(Arena* a, f64Arr input)
{
  DftResult res = { 0 };
  

  double N = (double)input.count;

  res.seq.capacity = input.capacity;
  res.seq.count = 0;
  res.seq.data = arena_alloc(a, Complex, input.count);
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

    // 100 samples per sec / 4096 = 0.0244140625 hz per bucket
    // 2000 / 6000 =  0.3 hz per sample 
    Complex next = { 0 };
    next.r = real;
    next.i = imag;
    Sequence_add(a, &res.seq, next);
    
  }  

  calc_peaks(a, &res);
  return res;
}


f64Arr dft_inv(Arena* a, Sequence input)
{
  f64Arr res = { 0 };

  isize N = input.count  * 2;

  res.capacity = input.capacity;
  res.count = 0;
  res.data = arena_alloc(a, f64, N);
   
  for (isize n = 0; n < N; n++)
  {

    Complex r = { 0 };
    
    // maybe we need to go to N, but it seems like we can just do the first half
    for (isize k = 0; k < N/2; k++)
    {
      Complex x_k = input.data[k];
      double angle = 2.0 * PI * k * n / N;
      Complex c = { 0 };
      c.r = cos(angle);
      c.i= sin(angle);
      Complex tmp = c_mul(x_k, c);
      r = c_add(tmp, r);
    }


    f64Arr_add(a, &res, r.r * 1.0 / N);

  }

  return res;
}


DftResult fft(Arena* perm_arena, Arena* tmp_arena, f64Arr input)
{
  Sequence tmp_res = fft_c_t(input, tmp_arena, input.count);

  DftResult res = { 0 };
  res.seq = Sequence_clone(perm_arena, tmp_res);
  calc_peaks(perm_arena, &res);  

  return res;
}

void calc_peaks(Arena *a, DftResult* res) 
{
  res->peak_frequencies = PeakFreqArray_empty(a, 5);
  for (i32 i = 0; i < res->seq.count / 2; i++)
  {

    Complex next = res->seq.data[i];
    // add peaks to output
    f64 mag = c_mag(next);
    if ( mag > 1000)
    {
      double freq = i * sample_rate / res->seq.capacity;
      PeakFreqArray_add(a, &res->peak_frequencies, (PeakFreq) { i, freq });
    }
  }
}


// see https://github.com/0xb01u/pyFFT/blob/master/Cooley-Tukey.py
// and https://github.com/bubnicbf/Fast-Fourier-Transform-using-Cooley-Tukey-algorithm/blob/master/FFT.cpp  
//coley turkey fft  
Sequence fft_c_t(f64Arr input, Arena* tmp_arena, isize N)
{
  // TODO MAKE SURE IT IS 0 initialized
  Sequence output = Sequence_empty(tmp_arena, N);

  memset(output.data, 0, output.capacity * sizeof(Complex));
  output.count = N;
  if (N == 1)
  {
    output.data[0] = (Complex) { input.data[0], 0.0 };
    return output;
  }

  // divide and conquer

  // assume N is power of 2, and thus equal
  f64Arr input_even = f64Arr_empty(tmp_arena, N / 2);
  f64Arr input_odd = f64Arr_empty(tmp_arena, N / 2);
  for (i32 i = 0; i < N / 2; i++)
  {
    f64Arr_add(tmp_arena, &input_even, input.data[i * 2]);
    f64Arr_add(tmp_arena, &input_odd, input.data[i * 2 + 1]);
  }

  Sequence even = fft_c_t(input_even, tmp_arena, N / 2);
  Sequence odd = fft_c_t(input_odd, tmp_arena, N / 2);

  
  // combine
  for (isize k = 0; k < N / 2; k++)
  {
    Complex w = { 0 };
    double angle = 2 * PI * k / N;
    w.r = cos(angle);
    w.i = -sin(angle);
    output.data[k] = c_add(even.data[k], c_mul(odd.data[k], w));
    output.data[k + N/2] = c_sub(even.data[k], c_mul(odd.data[k], w));
  }
  
  return output;

}


void remove_freq(DftResult* dft_res, PeakFreq peak_f)
{
  dft_res->seq.data[peak_f.index] = (Complex){ 0.0, 0.0 };

  update_plot_data();  
  calc_peaks(&ctx.perm_arena, &ctx.dft_res);

}

f64Arr gen_wave_test(Arena* a)
{
  // sample 3 sec
  // wave is 1 hz
  // Sample 10 sec, with sample rate is number of samles
  isize samples = (isize)powl(2, 12);
  f64Arr res = f64Arr_empty(a, samples);

  // each sample is 1/samples of a sec
  f64 step = 2.0 * PI / sample_rate;
  
  for (isize i = 0; i < res.capacity; i++)
  {
    f64 v = 2.0 * sin(wave_freq * i * step);

    v += sin((wave_freq + 10) * i * step);
    

    f64Arr_add(a, &res, v);
  }

  return res;
}

f64Arr gen_wave(Arena* a, double freq, isize sample_freq_khz, isize samples)
{
  f64Arr res = f64Arr_empty(a, samples);

  

  return res;
}