#include "window.h"


LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);
#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif


typedef void (*quit_fn) ();
typedef void (*draw_fn) ();

quit_fn quit_f;
draw_fn draw_f;
 

HWND NewWindow(HINSTANCE hInstance, quit_fn qf, draw_fn df) {
  // WINDOW SETUP
  const wchar_t window_class_name[] = L"My Window Class";
  static WNDCLASS window_class = { 0 };
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClass(&window_class);

  quit_f = qf;
  draw_f = df;

  static HWND window_handle;
  window_handle = CreateWindow(window_class_name, L"Drawing Pixels", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    640, 300, 640, 480, NULL, NULL, hInstance, NULL);
  if (window_handle == NULL) { return window_handle; }

  return window_handle;
}


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
    draw_f(window_handle);
  } break;
  /*
case WM_PAINT: {
  static PAINTSTRUCT paint;
  static HDC device_context;
  device_context = BeginPaint(window_handle, &paint);

  Graphics graphics(device_context);

  // draw rect to clear screen
  RECT window_rect;
  GetWindowRect(window_handle, &window_rect);

  //Rectangle(hDC, window_rect.left, window_rect.top, window_rect.right, window_rect.bottom);

  FillRect(device_context, &window_rect, (HBRUSH)(COLOR_WINDOW + 1));



  DrawCursorColor(device_context);


  EndPaint(window_handle, &paint);
} break;
*/

  default: {
    return DefWindowProc(window_handle, message, wParam, lParam);
  }
  }
  return 0;
}