#pragma once


typedef struct
{
  double r;
  double i;
} Complex;


typedef struct {
  isize count;
  isize capacity;
  Complex* data;
} Sequence;
