#include "window.h"
#include <windowsx.h>

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);
#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

static w_window *window;

w_window* W_NewWindow(HINSTANCE hInstance) {
  // WINDOW SETUP
  const wchar_t window_class_name[] = L"My Window Class";
  static WNDCLASS window_class = { 0 };
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClass(&window_class);
  window = malloc(sizeof(w_window));

  window->handle = CreateWindow(window_class_name, L"Drawing Pixels", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    640, 300, 640, 480, NULL, NULL, hInstance, NULL);  
  return window;

}


static RL_RenderCommands* W_commands;

static w_mouse _mouse;

w_mouse w_mouse_state()
{
  return _mouse;
}

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, u32 message, WPARAM wParam, LPARAM lParam) {

  switch (message) {
  case WM_QUIT:
  case WM_DESTROY: {
    window->quit_f();
  } break;
  case WM_PAINT:
  {
    if (W_commands != 0)
    {
      window->draw_f(window_handle, W_commands);
    }
    else {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window_handle, &ps);
      EndPaint(window_handle, &ps);
    }
  } break;
  case WM_MOUSEMOVE:
  {
    _mouse.x = GET_X_LPARAM(lParam);
    _mouse.y = GET_Y_LPARAM(lParam);
  }
  default: {
    return DefWindowProc(window_handle, message, wParam, lParam);
  }
  }
  return 0;
}

void w_frame_end(RL_RenderCommands* commands)
{
  W_commands = commands;
  InvalidateRect(window->handle, NULL, TRUE);
  UpdateWindow(window->handle);

}

