#include <windows.h>
#include "render_layer.h"
#include "arena.h"
#include "s_string.h"
#include "window.h"

//

// array of commands
typedef struct {
  RL_RenderCommand* commands;
  size len;
} RenderCommands;

static RenderCommands commands;

void rl_push_rectangle()
{

}



void rl_start_frame(Arena* arena)
{
  // 1024 commands
  commands.commands = arena_alloc(arena, RL_RenderCommand, 1024);  
  commands.len = 0;
}

void rl_end_frame()
{
  // call platform layer
  frame_end(commands.commands, commands.len);
}

// push text output to command buffer
void rl_push_text(Arena* arena, s16 text)
{  
  size next = commands.len;
  RL_RenderCommand* command = &commands.commands[next];
  commands.len += 1;

  command->commandType = RL_TEXT;
  command->text = text; 

}


