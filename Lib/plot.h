#pragma once

#include "custom_types.h"
#include "arena.h"
#include "raylib.h"


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

typedef struct
{
  DrawInfo info;
} Plot;

typedef struct
{
  Color color;
  void (*draw_elm)(int, int, DrawInfo, Color);
  isize count;
  f64* data;  
} PlotData;

/// <summary>
/// Updates info in plot, with the data that will be plotted
/// Calc min and max for x and y. Index om min and max y value
/// </summary>
/// <param name="plot">Pointer to a plot to be updated</param>
/// <param name="plot_data">Data to be plotted</param>
void pl_update_plot_info(Plot* plot, PlotData* plot_data);

void pl_plot(Arena* frame_arena, Plot p, PlotData pd);

void pl_draw_dot_fn(int x, int y, DrawInfo info, Color color);

void pl_draw_dft_fn(int x, int y, DrawInfo info, Color color);