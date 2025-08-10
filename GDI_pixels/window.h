#include <windows.h>
#include "render_layer.h"
#include "custom_types.h"

typedef void (*quit_fn) ();
typedef void (*draw_fn) ();

HWND W_NewWindow(HINSTANCE hInstance, quit_fn qf, draw_fn df);

void frame_end(RL_RenderCommand* commands, i32 commands_len);