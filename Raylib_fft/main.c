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
#include "fft.h"


typedef struct {
  Sequence seq;
  PeakFreqArray peak_frequencies;
  f64 N;
} DftResult;

typedef struct {
  Arena frame_arena;
  Arena perm_arena;
  Sequence test;
  DftResult dft_res;
  b32 draw_fft;
  b32 use_dft;
  Plot plot;
  PlotData plot_data;
  Sequence dft_plot_data;
  i64 operations;
} Context;


Sequence gen_wave(Arena* a, f64 freq, isize sample_freq_khz, isize samples);
Sequence gen_wave_test(Arena* a);

DftResult transform(Sequence input);
void calc_peaks(Arena* a, DftResult* res);

f64 c_mag(Complex c);

f64Arr magnitude_arr(Arena* a, Sequence s);

f64Arr real_arr(Arena* a, Sequence s);

void remove_freq(DftResult* dft_res, PeakFreq peak_f);

void initialize_context();
void update_plot_data();

void draw_frequencies(f32 next_y);
void set_plot_data(PlotData *p, f64Arr data)
{
  p->count = data.count; 
  p->data = data.data;
}

f64 sample_rate = 2048;
f32 wave_freq = 1.0; 


Context ctx = { 0 };

Rectangle panelRec = { 20, 40, 200, 150 };
Rectangle panelContentRec = { 0, 0, 80, 100 };
Rectangle panelView = { 0 };
Vector2 panelScroll = { 99, -20 };

const i32 screenWidth = 1200;
const i32 screenHeight = 800;
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------


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

    if (GuiButton((Rectangle) { 10, next_button_y, 80, 40 }, "Calc peaks"))
    {
      calc_peaks(&ctx.perm_arena, &ctx.dft_res);
    }
    next_button_y += button_y_inc;

    if (GuiButton((Rectangle) { 10, next_button_y, 80, 40 }, "Inverse_dft"))
    {     
      // to do inverse switch algo

      //ctx.test = dft_inv(&ctx.perm_arena, ctx.dft_res.seq);
      ctx.test = fft_ifft(&ctx.perm_arena, &ctx.frame_arena, ctx.dft_res.seq);
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

    pl_plot(ctx.plot);

    if (ctx.dft_res.peak_frequencies.count > 0)
    {
      draw_frequencies(next_button_y);
     
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

void draw_frequencies(f32 next_button_y)
{
  s8 number_s = s8_isize_to_s8(&ctx.frame_arena, ctx.dft_res.peak_frequencies.count);
  s8 freq_s = s8_concat(&ctx.frame_arena, s8_from_literal("Peak frequencies ("), number_s);
  s8_append_c_str(&ctx.frame_arena, &freq_s, ") ");
  

  f32 button_y_inc = 50;

  panelContentRec.height = ctx.dft_res.peak_frequencies.count * button_y_inc ;

  panelView.x = 10;
  panelView.y = next_button_y;
  panelView.height = min(screenHeight - 10 - panelView.y, panelContentRec.height + 15);
  panelView.width = 100;

  panelRec.x = 10;
  panelRec.y = next_button_y ;
  panelRec.height = panelView.height;
  panelRec.width = 100;

  // the rect that is visible in on screen
  GuiScrollPanel(panelView, NULL, panelContentRec, &panelScroll, &panelView);

  BeginScissorMode(panelView.x, panelView.y, panelView.width, panelView.height);
    
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

    if (GuiButton((Rectangle) { (f32)10, next_button_y + panelScroll.y, (f32)80, (f32)40 }, button_text.data))
    {
      remove_freq(&ctx.dft_res, ctx.dft_res.peak_frequencies.data[i]);
    }
    next_button_y += button_y_inc;
  }

  EndScissorMode();

  s8_append_zero(&ctx.frame_arena, &freq_s);
  DrawText(freq_s.data, 300, 20, 20, BLACK);
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
  set_plot_data(&plot_data, real_arr(&ctx.perm_arena, ctx.test));
  plot_data.draw_elm = &pl_draw_dot_fn;
  pl_update_plot_info(&ctx.frame_arena, &plot, &plot_data);
  
  ctx.plot = plot;
  ctx.plot_data = plot_data;
}

void update_plot_data()
{

  if (ctx.draw_fft)
  {
    set_plot_data(&ctx.plot_data, magnitude_arr(&ctx.perm_arena, ctx.dft_res.seq));
    ctx.plot_data.draw_elm = &pl_draw_dft_fn;
  }
  else
  {
    set_plot_data(&ctx.plot_data, real_arr(&ctx.perm_arena, ctx.test));
    ctx.plot_data.draw_elm = &pl_draw_dot_fn;
  }

  pl_update_plot_info(&ctx.frame_arena, &ctx.plot, &ctx.plot_data);
}

f64Arr real_arr(Arena* a, Sequence s)
{
  // just get real data 
  f64Arr res = f64Arr_empty(a, s.count);

  res.count = s.count;
  for (isize i = 0; i < s.count; i++)
  {
    res.data[i] = s.data[i].r;
  }
  return res;
}

f64Arr magnitude_arr(Arena* a, Sequence s)
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

DftResult transform(Sequence input)
{
  DftResult res = { 0 };
  if (ctx.use_dft)
  {
    res.seq = fft_dft(&ctx.perm_arena, input);
  }
  else {
    res.seq = fft_fft(&ctx.perm_arena, &ctx.frame_arena, input);
    res.seq.count = res.seq.count / 2;
  }

  
  res.N = (f64)input.count;
  calc_peaks(&ctx.perm_arena, &res);

  return res;
}

void calc_peaks(Arena *a, DftResult* res) 
{
  
  res->peak_frequencies = PeakFreqArray_empty(a, 5);
  for (i32 i = 0; i < res->seq.count; i++)
  {    
    Complex next = res->seq.data[i];
    // add peaks to output
    f64 mag = c_mag(next);
    if ( mag > 0.3)
    {
      double freq = i * sample_rate / res->seq.capacity;
      PeakFreqArray_add(a, &res->peak_frequencies, (PeakFreq) { i, freq });
    }
  }
}

void remove_freq(DftResult* dft_res, PeakFreq peak_f)
{
  dft_res->seq.data[peak_f.index] = (Complex){ 0.0, 0.0 };

  update_plot_data();  
  calc_peaks(&ctx.perm_arena, &ctx.dft_res);

}

Sequence gen_wave_test(Arena* a)
{
  // sample 3 sec
  // wave is 1 hz
  // Sample 10 sec, with sample rate is number of samles
  isize samples = (isize)powl(2, 16);
  Sequence res = Sequence_empty(a, samples);

  // each sample is 1/samples of a sec
  f64 step = 2.0 * PI / sample_rate;
  for (isize i = 0; i < res.capacity; i++)
  {
    Complex v = { 0 };
    for (isize j = 0; j < 20; j++)
    {
      v.r += 2.0 * sin((wave_freq + j ) * i * step);      
    }
    Sequence_add(a, &res, v);
  }

  return res;
}

Sequence gen_wave(Arena* a, double freq, isize sample_freq_khz, isize samples)
{
  Sequence res = Sequence_empty(a, samples);

  return res;
}