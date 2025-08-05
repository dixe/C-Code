
#include "window.h"


LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);
#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

HWND NewWindow(HINSTANCE hInstance) {
  // WINDOW SETUP
  const wchar_t window_class_name[] = L"My Window Class";
  static WNDCLASS window_class = { 0 };
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClass(&window_class);


  static HWND window_handle;
  window_handle = CreateWindow(window_class_name, L"Drawing Pixels", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    640, 300, 640, 480, NULL, NULL, hInstance, NULL);
  if (window_handle == NULL) { return window_handle; }

  return window_handle;
}