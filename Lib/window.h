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
  i32 x;
  i32 y;
  // color
  // position
  // size?
  // rect?
} RL_RenderCommand;

typedef struct {
  RL_RenderCommand* commands;
  isize byte_len;
} RL_RenderCommands;


typedef void (*quit_fn) ();
typedef void (*draw_fn) (HWND, RL_RenderCommands*);

typedef struct {
  HWND handle;
  quit_fn quit_f;
  draw_fn draw_f;
} w_window;


typedef struct  {
  i32 x;
  i32 y;
} w_mouse;

w_window* W_NewWindow(HINSTANCE hInstance);

void w_frame_end(RL_RenderCommands* commands);

w_mouse w_mouse_state();