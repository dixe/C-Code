#include "plot.h"
#include <raylib.h>
#include "s_string.h"

void pl_update_plot_info(Plot* plot, PlotData* plot_data)
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
}

void pl_plot(Arena* frame_arena,  Plot p, PlotData pd)
{

  DrawRectangle(p.info.x_base, p.info.y_base, p.info.w, p.info.h, WHITE);

  // draw line at y = 0;
  double y0 = 1.0 - (0 - p.info.y_min) / (p.info.y_max - p.info.y_min);

  // draw 0 at 0 line
  int y_0 = (int)(y0 * p.info.h + p.info.y_base);
  DrawText("0", 100, y_0 - 8, 16, BLACK);

  // draw max at top
  s8 number_s = s8_f64_to_s8(frame_arena, p.info.y_max, 2);
  s8_append_zero(frame_arena, &number_s);
  DrawText(number_s.data, p.info.x_base - 20, p.info.y_base - 20, 16, BLACK);


  //draw min at bottom
  number_s = s8_f64_to_s8(frame_arena, p.info.y_min, 2);
  s8_append_zero(frame_arena, &number_s);
  DrawText(number_s.data, p.info.x_base - 20, p.info.y_base + p.info.h + 20, 16, BLACK);


  DrawLine(p.info.x_base, y_0, p.info.x_base + p.info.w, y_0, BLACK);

  for (isize i = 0; i < pd.count; i++)
  {
    double m = pd.data[i];

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

void pl_draw_dot_fn(int x, int y, DrawInfo info)
{
  DrawCircle(x, y, 2, RED);
}

void pl_draw_dft_fn(int x, int y, DrawInfo info)
{
  DrawLine(x, y, x, info.y_base + info.h, RED);
  DrawCircle(x, y, 2, RED);
}