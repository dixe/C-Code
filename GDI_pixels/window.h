#include <windows.h>
#include "render_layer.h"

typedef void (*quit_fn) ();
typedef void (*draw_fn) ();

HWND W_NewWindow(HINSTANCE hInstance, quit_fn qf, draw_fn df);

void frame_end(RL_RenderCommand* commands, int commands_len);