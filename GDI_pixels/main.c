#ifndef UNICODE 
  #define UNICODE
#endif
#ifndef _UNICODE 
  #define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include "window.h"
#include "gdi_renderer.h"
#include "render_layer.h"
#include "arena.h"
#include "custom_types.h"
#include "s_string.h"

// based on code from https://www.youtube.com/watch?v=q1fMa8Hufmg

static bool quit = false;

COLORREF getCusorColor()
{
  POINT pt;

  GetCursorPos(&pt);
  HDC hDC = GetDC(NULL);
  COLORREF color = GetPixel(hDC, pt.x, pt.y);
  ReleaseDC(GetDesktopWindow(), hDC);
  return color;
}

void OnQuit() {
  quit = true;
}


// could be program logic, maybe getCursor should use platform layer?
void DrawCursorColor(Arena* arena) {
  COLORREF color = getCusorColor();

  i32 r = GetRValue(color);
  i32 g = GetGValue(color);
  i32 b = GetBValue(color);

  s16 s = s16_empty(arena, 64);
  // maybe macro so we can just do s_format(s, L"Points: (%i, %i, %i)", r, g, b);
  // but this is ok
  s.len = swprintf_s(s.data, s.capacity, L"Points: (%i, %i, %i)", r, g, b);

  w_mouse m = w_mouse_state();
  rl_push_text(arena, s, m.x, m.y);
}


i32 _stdcall WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, i32 nCmdShow) {

  // WINDOW SETUP
  w_window* window_handle;
  window_handle = W_NewWindow(hInstance);
  window_handle->quit_f = OnQuit;

  rl_setup(window_handle);

  Arena frameArena = arena_create(1024*1024); // 1 mb of ram for frame arena

  while (!quit) {
    rl_start_frame(&frameArena);

    // maybe for platform/window layer?
    static MSG message = { 0 };
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
    { 
      DispatchMessage(&message);
    }


    DrawCursorColor(&frameArena);

    // for rendering layer
    rl_end_frame();
    arena_reset(&frameArena);
    // reset framea
  }

  return 0;
}

