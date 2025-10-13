#include "plot.h"
#include <raylib.h>
#include "s_string.h"


void pli_plot_to_texture(Arena* frame_arena, Plot* p, PlotData* pd);

void pl_update_plot_info(Arena* frame_arena, Plot* plot, PlotData* plot_data)
{
  plot->info.y_max_index = -1;
  plot->info.y_min_index = -1;

  plot->info.x_min = 1000000000;
  plot->info.y_min = 1000000000;
  plot->info.x_max = -1000000000;
  plot->info.y_max = -1000000000;

  for (isize i = 0; i < plot_data->count; i++)
  {
    double m = plot_data->data[i];

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
    plot->info.x_max = (double)i > plot->info.x_max ? (double)i : plot->info.x_max;
    plot->info.x_min = (double)i < plot->info.x_min ? (double)i : plot->info.x_min;
  }

  if (plot->target.id == 0)
  {

    plot->target = LoadRenderTexture(plot->info.w, plot->info.h);
  }

  BeginTextureMode(plot->target);
  pli_plot_to_texture(frame_arena, plot, plot_data);
  EndTextureMode();
}

void pl_plot(Plot p)
{
  // render texture 
  DrawTextureRec(p.target.texture, (Rectangle) { 0, 0, (float)p.target.texture.width, (float)-p.target.texture.height }, (Vector2) { (f32)p.info.x_base, (f32)p.info.y_base}, WHITE);

}

// this draw to a texture
void pli_plot_to_texture(Arena* frame_arena, Plot *p, PlotData *pd)
{
  DrawRectangle(0 ,0, p->info.w, p->info.h, WHITE);

  // draw line at y = 0;
  double y0 = 1.0 - (0 - p->info.y_min) / (p->info.y_max - p->info.y_min);

  // draw 0 at 0 line
  int y_0 = (int)(y0 * p->info.h);
  DrawText("0",0, y_0 - 16, 16, BLACK);

  // draw max at top
  s8 number_s = s8_f64_to_s8(frame_arena, p->info.y_max, 2);
  s8_append_zero(frame_arena, &number_s);
  DrawText(number_s.data, 0, 0, 16, BLACK);


  //draw min at bottom
  number_s = s8_f64_to_s8(frame_arena, p->info.y_min, 2);
  s8_append_zero(frame_arena, &number_s);
  DrawText(number_s.data, 0, p->info.h + 20, 16, BLACK);


  DrawLine(0, y_0, p->info.w, y_0, BLACK);

  // offset to have space for y axis numbers
  i32 x_offset = 35;
  for (isize i = 0; i < pd->count; i++)
  {
    double m = pd->data[i];

    // range lerp
    // 0 - 1 range, inversed
    double x = (i - p->info.x_min) / (p->info.x_max - p->info.x_min);
    x = x * (p->info.w - x_offset - 5);

    // 0 - 1 range, inversed since screen space coordinate system i 0,0 at top left, and not bottom
    double y = 1.0 - (m - p->info.y_min) / (p->info.y_max - p->info.y_min);
    y = y * p->info.h;

    pd->draw_elm((int)x + x_offset, (int)y, p->info, pd->color);
  }
}

void pl_draw_dot_fn(int x, int y, DrawInfo info, Color color)
{
  DrawCircle(x, y, 2, color);
}

void pl_draw_dft_fn(int x, int y, DrawInfo info, Color color)
{
  DrawLine(x, y, x, info.y_base + info.h, RED);
  DrawCircle(x, y, 2, color);
}