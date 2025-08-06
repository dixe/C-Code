# pragma once




typedef enum  {
  RL_RECTANGLE,
  RL_TEXT
} RL_RenderCommandType;

typedef struct 
{
  enum RL_RenderCommandType commandType;
  wchar_t* text;
  int text_len;
  // color
  // position
  // size?
  // rect?
} RL_RenderCommand;



// push a rectangle to the comamnd buffer
void rl_push_rectangle();

// push text output to command buffer
RL_RenderCommand* rl_push_text(wchar_t* text, int len);
