#pragma once 

#include <windows.h>
#include "custom_types.h"
#include "s_string.h"



typedef enum {
  RL_RECTANGLE,
  RL_TEXT
} RL_RenderCommandType;



typedef struct
{
  RL_RenderCommandType commandType;
  s16 text;
  // color
  // position
  // size?
  // rect?
} RL_RenderCommand;

typedef struct {
  RL_RenderCommand* commands;
  xsize len;
} RL_RenderCommands;


typedef void (*quit_fn) ();
typedef void (*draw_fn) (HWND, RL_RenderCommands*);

typedef struct {
  HWND handle;
  quit_fn quit_f;
  draw_fn draw_f;
} w_window;



w_window* W_NewWindow(HINSTANCE hInstance);

void w_frame_end(RL_RenderCommands* commands);