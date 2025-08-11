# pragma once
#include "arena.h"
#include "string.h"


typedef enum  {
  RL_RECTANGLE,
  RL_TEXT
} RL_RenderCommandType;

typedef struct 
{
  RL_RenderCommandType commandType;
  s_string text;
  // color
  // position
  // size?
  // rect?
} RL_RenderCommand;



// allocate render command buffer.
// maybe just pass
void rl_start_frame(Arena* arena);


// push a rectangle to the comamnd buffer
void rl_push_rectangle();

// push text output to command buffer
RL_RenderCommand* rl_push_text(Arena* arena, s_string text);



void rl_end_frame();