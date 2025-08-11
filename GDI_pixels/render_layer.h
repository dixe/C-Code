# pragma once

#include "arena.h"
#include "s_string.h"
#include "window.h"



// allocate render command buffer.
// maybe just pass
void rl_start_frame(Arena* arena);

void rl_setup(w_window* w);

// push a rectangle to the comamnd buffer
void rl_push_rectangle();

// push text output to command buffer
void rl_push_text(Arena* arena, s16 text);



void rl_end_frame();