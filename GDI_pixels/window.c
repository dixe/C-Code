#include "window.h"


LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);
#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif


typedef void (*quit_fn) ();
typedef void (*draw_fn) (RL_RenderCommand* commands, int commands_len);

quit_fn quit_f;
draw_fn draw_f;


static HWND window_handle;

HWND W_NewWindow(HINSTANCE hInstance, quit_fn qf, draw_fn df) {
  // WINDOW SETUP
  const wchar_t window_class_name[] = L"My Window Class";
  static WNDCLASS window_class = { 0 };
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClass(&window_class);

  quit_f = qf;
  draw_f = df;

  window_handle = CreateWindow(window_class_name, L"Drawing Pixels", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    640, 300, 640, 480, NULL, NULL, hInstance, NULL);
  if (window_handle == NULL) { return window_handle; }

  return window_handle;
}


static RL_RenderCommand* W_commands;
static int W_commands_len;

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {

  PAINTSTRUCT paint;
  HDC hdc;

  switch (message) {
  case WM_QUIT:
  case WM_DESTROY: {
    quit_f();
  } break;
  case WM_PAINT:
  {
    draw_f(window_handle, W_commands, W_commands_len);
  } break;
  default: {
    return DefWindowProc(window_handle, message, wParam, lParam);
  }
  }
  return 0;
}

void frame_end(RL_RenderCommand* commands, int commands_len)
{
  W_commands = commands;
  W_commands_len = commands_len;
  InvalidateRect(window_handle, NULL, TRUE);
  UpdateWindow(window_handle);

}