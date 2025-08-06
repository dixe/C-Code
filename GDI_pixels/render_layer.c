#include <windows.h>
#include "render_layer.h"
// push a rectangle to the comamnd buffer
void rl_push_rectangle();



static RL_RenderCommand* command;
// push text output to command buffer
RL_RenderCommand* rl_push_text(wchar_t* text, int len)// , arena* arena_allocator
{
  if (command == NULL)
  {
    command = malloc(sizeof(command));
  }

  command->commandType = RL_TEXT;
  command->text = text;
  command->text_len = len;

  return command;
}


