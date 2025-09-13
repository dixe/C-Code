/*******************************************************************************************
*
*   raylib [text] example - rectangle bounds
*
*   Example complexity rating: [★★★★] 4/4
*
*   Example originally created with raylib 2.5, last time updated with raylib 4.0
*
*   Example contributed by Vlad Adrian (@demizdor) and reviewed by Ramon Santamaria (@raysan5)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2018-2025 Vlad Adrian (@demizdor) and Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "arena.h"
#include "hashmap.h"
#include "types.h"
#include "containers.h"

Sequence dft(Arena* a, Sequence input);
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Fourier");
  //--------------------------------------------------------------------------------------

  
  Arena frame_arena = arena_create(512);

  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {
    Sequence s = sequence_empty(&frame_arena, 4);

    Complex c0 = { 1,0 };
    Complex c1 = { 2, -1 };
    Complex c2 = { 0, -1 };
    Complex c3 = { -1, 2 };

    sequence_add(&frame_arena, &s, c0);
    sequence_add(&frame_arena, &s, c1);
    sequence_add(&frame_arena, &s, c2);
    sequence_add(&frame_arena, &s, c3);


    Sequence res = dft(&frame_arena, s);


    BeginDrawing();

    ClearBackground(RAYWHITE);

    EndDrawing();
    arena_reset(&frame_arena);
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

//--------------------------------------------------------------------------------------
// Module functions definition
//--------------------------------------------------------------------------------------

Complex c_mul(Complex c1, Complex c2)
{
  Complex res = { 0 };
  double a = c1.r;
  double b = c1.i;
  double c = c2.r;
  double d = c2.i;

  res.r = a * c - b * d;
  res.i = a * d + b * c;

  return res;
}

Complex c_add(Complex a, Complex b)
{
  Complex c = { a.r + b.r, a.i + b.i };
  return c;
     
}


Sequence dft(Arena* a, Sequence input) 
{
  Sequence res = { 0 };


  double N = input.count;

  res.capacity = input.capacity;
  res.count = 0;
  res.data = arena_alloc(a, Complex,N);

  
  for (isize k = 0; k < N; k++)
  {
    Complex next = { 0 };
    for (isize i = 0; i < N; i++)
    {
      double n = i;    

      Complex c = { 0 };
      double exp = -2 * PI * (k / N) * n;
      c.r = cos(exp);
      c.i = sin(exp);
      Complex prod = c_mul(input.data[i], c);
      next = c_add(next, prod);
    }
    sequence_add(a, &res, next);
  }  

  return res;
}