#include <windows.h>
#include "render_layer.h"
#include "arena.h"
#include "s_string.h"
#include "window.h"

//

// array of commands


static RL_RenderCommands commands;



void rl_vm_paint(HWND window_handle, RL_RenderCommands* commands) {

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
  for (i32 i = 0; i < commands->len; i++)
  {
    RL_RenderCommand command = commands->commands[i];
    switch (command.commandType)
    {
    case RL_RECTANGLE:
      break;
    case RL_TEXT:
      TextOutW(memDC, command.x, command.y, command.text.data, (i32)command.text.len);
      break;
    }
  }

  // Blit the memory DC to the screen
  BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

  // Clean up
  SelectObject(memDC, oldBitmap);
  DeleteObject(memBitmap);
  DeleteDC(memDC);

  EndPaint(window_handle, &ps);
}


void rl_setup(w_window* window)
{
  window->draw_f = rl_vm_paint;
}



void rl_push_rectangle()
{

}

void rl_start_frame(Arena* arena)
{
  // 1024 commands
  commands.commands = arena_alloc(arena, RL_RenderCommand, 1024);  
  commands.len = 0;
}

void rl_end_frame()
{
  // call platform layer
  w_frame_end(&commands);
}

// push text output to command buffer
void rl_push_text(Arena* arena, s16 text, i32 x, i32 y)
{  
  isize next = commands.len;
  RL_RenderCommand* command = &commands.commands[next];
  commands.len += 1;

  command->commandType = RL_TEXT;
  command->text = text; 
  command->x = x;
  command->y = y; 
}