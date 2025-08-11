# pragma once
#include "arena.h"
#include "s_string.h"


typedef enum  {
  RL_RECTANGLE,
  RL_TEXT
} RL_RenderCommandType;

typedef struct 
{
  RL_RenderCommandType commandType;
  s16 text;
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
void rl_push_text(Arena* arena, s16 text);



void rl_end_frame();