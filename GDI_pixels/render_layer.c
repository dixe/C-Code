#include <windows.h>
#include "render_layer.h"
#include "arena.h"
// push a rectangle to the comamnd buffer
void rl_push_rectangle();



static RL_RenderCommand* command;
// push text output to command buffer
RL_RenderCommand* rl_push_text(Arena* arena, wchar_t* text, int len)// , arena* arena_allocator
{

  command = (RL_RenderCommand*) arena_alloc(arena, sizeof(command));

  command->commandType = RL_TEXT;
  command->text = text;
  command->text_len = len;

  return command;
}


