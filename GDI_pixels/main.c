#ifndef UNICODE 
  #define UNICODE
#endif
#ifndef _UNICODE 
  #define _UNICODE
#endif

#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include "window.h"
#include <stdio.h>
#include "gdi_renderer.h"
#include "render_layer.h"
#include "arena.h"
#include "custom_types.h"
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

static int reduce = 10000;


void VmPaint(HWND window_handle, RL_RenderCommand* commands, i32 num_commands);
void OnQuit() {
  quit = true;
}

RL_RenderCommand* DrawCursorColor(Arena* arena);

i32 _stdcall WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, i32 nCmdShow) {

  // WINDOW SETUP
  static HWND window_handle;
  window_handle = W_NewWindow(hInstance, OnQuit, VmPaint);

  Arena* frameArena = create_arena(1024*1024); // 1 mb of ram for frame arena

  while (!quit) {

    // maybe for platform/window layer?
    static MSG message = { 0 };
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
    { 
      DispatchMessage(&message);
    }


    RL_RenderCommand* command = DrawCursorColor(frameArena);

    reduce -= 1;

    // for rendering layer
    frame_end(command, 1);
    arena_reset(frameArena);
    // reset framea
  }

  return 0;
}



// could be program logic, maybe getCursor should use platform layer?
// should use arena to allocate the text
RL_RenderCommand* DrawCursorColor(Arena* arena) {
  COLORREF color = getCusorColor();

  i32 r = GetRValue(color);
  i32 g = GetGValue(color);
  i32 b = GetBValue(color);

  s_string s = s_empty(arena, 64);
  // maybe macro so we can just do s_format(s, L"Points: (%i, %i, %i)", r, g, b);
  // but this is ok
  s.len = swprintf_s(s.data, s.capacity, L"Points: (%i, %i, %i)", r, g, b);

  RL_RenderCommand* res = rl_push_text(arena, s);

  return res;
}



void VmPaint(HWND window_handle, RL_RenderCommand* commands, i32 num_commands ) {

  // should be in gdi renderer or maybe call it?
  // should run over all render commands and do them

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(window_handle, &ps);
  // Create a memory DC compatible with the screen
  HDC memDC = CreateCompatibleDC(hdc);
  RECT clientRect;
  GetClientRect(window_handle, &clientRect);

  // Create a bitmap compatible with the window DC
  HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
  HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

  // Fill background
  HBRUSH bgBrush = (HBRUSH)(COLOR_WINDOW + 1);
  FillRect(memDC, &clientRect, bgBrush);

  // Draw text into the memory DC
  for (i32 i = 0; i < num_commands; i++)
  {
    switch (commands[i].commandType)
    {
      case RL_RECTANGLE:
        break;
      case RL_TEXT:
        TextOutW(memDC, 0, 0, commands[i].text.data, (i32) commands[i].text.len);
        break;
      }
  }

  // OLD
 /*
  
  if (reduce > 0)
  {
    // push_text
    TextOutW(memDC, 0, 0, L"Hello, Windows !", 15);
  }
  else
  {
    TextOutW(memDC, 0, 0, L"JHello !", 8); // Correct length
  }

  */


  // Blit the memory DC to the screen
  BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

  // Clean up
  SelectObject(memDC, oldBitmap);
  DeleteObject(memBitmap);
  DeleteDC(memDC);

  EndPaint(window_handle, &ps);
}

