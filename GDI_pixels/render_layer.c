#include <windows.h>
#include "render_layer.h"
#include "arena.h"
#include "string.h"

// push a rectangle to the comamnd buffer
void rl_push_rectangle();



static RL_RenderCommand* command;
// push text output to command buffer
RL_RenderCommand* rl_push_text(Arena* arena, s_string text)
{

  command = (RL_RenderCommand*) arena_alloc(arena, sizeof(command));

  command->commandType = RL_TEXT;
  command->text = text;

  return command;
}


