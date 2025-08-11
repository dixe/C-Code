#include "window.h"


LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);
#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif


typedef void (*quit_fn) ();
typedef void (*draw_fn) (RL_RenderCommand* commands, xsize commands_len);



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


static RL_RenderCommand* W_commands;
static xsize W_commands_len;

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, u32 message, WPARAM wParam, LPARAM lParam) {

  switch (message) {
  case WM_QUIT:
  case WM_DESTROY: {
    window->quit_f();
  } break;
  case WM_PAINT:
  {
    window->draw_f(window_handle, W_commands, W_commands_len);
  } break;
  default: {
    return DefWindowProc(window_handle, message, wParam, lParam);
  }
  }
  return 0;
}

void w_frame_end(RL_RenderCommand* commands, xsize commands_len)
{
  W_commands = commands;
  W_commands_len = commands_len;
  InvalidateRect(window->handle, NULL, TRUE);
  UpdateWindow(window->handle);

}

