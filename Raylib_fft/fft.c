#include "fft.h"
#include "raylib.h" // fofr PI
#include <string.h>
#include <math.h>


Sequence coley_turkey(Sequence input, Arena* tmp_arena, isize N, f64 exp_sign);


Complex c_mul(Complex c1, Complex c2)
{
  Complex res = { 0 };
  double r1 = c1.r;
  double i1 = c1.i;
  double r2 = c2.r;
  double i2 = c2.i;

  res.r = r1 * r2 - i1 * i2;
  res.i = r1 * i2 + i1 * r2;

  return res;
}

double c_mag(Complex c)
{
  return sqrt(c.r * c.r + c.i * c.i);
}

Complex c_add(Complex a, Complex b)
{
  Complex c = { a.r + b.r, a.i + b.i };
  return c;

}

Complex c_sub(Complex a, Complex b)
{
  Complex c = { a.r - b.r, a.i - b.i };
  return c;

}


// Only comutes the first half, since the rest is not use full when using real values, negative frequemcies
// for inverse we might need it again
Sequence fft_dft(Arena* a, Sequence input)
{
  double N = (double)input.count;

  Sequence res = Sequence_empty(a, input.count);

  // only compute the first N/2 values, since the rest are useless for realtime data
  for (isize k = 0; k < N / 2 + 1; k++)
  {

    f64 real = 0.0;
    f64 imag = 0.0;
    for (isize i = 0; i < N; i++)
    {
      f64 x_n = input.data[i].r;
      double n = (double)i;

      double angle = 2 * PI * k * n / N;
      real += x_n * cos(angle);
      imag += -x_n * sin(angle);
    }

    Complex next = { 0 };
    next.r = real;
    next.i = imag;
    Sequence_add(a, &res, next);
  }

  return res;
}

Sequence fft_dft_inv(Arena* a, Sequence input)
{
  isize N = input.count * 2;
  Sequence res = Sequence_empty(a, N);

  for (isize n = 0; n < N; n++)
  {
    Complex r = { 0 };

    // maybe we need to go to N, but it seems like we can just do the first half
    // we do multiply N with 2 at the start, so this is length of input sequence
    for (isize k = 0; k < N / 2; k++)
    {
      Complex x_k = input.data[k];
      double angle = 2.0 * PI * k * n / N;
      Complex c = { 0 };
      c.r = cos(angle);
      c.i = sin(angle);
      Complex tmp = c_mul(x_k, c);
      r = c_add(tmp, r);
    }


    Sequence_add(a, &res, c_mul(r, (Complex) { 1.0 / N, 0.0 }));

  }

  return res;
}


Sequence fft_ifft(Arena* perm_arena, Arena* tmp_arena, Sequence input)
{  
  isize N = input.count;
  Sequence tmp_res = coley_turkey(input, tmp_arena, input.count, 1.0);

  Sequence res = Sequence_empty(perm_arena, N);
  // multiply all results with 1/N
  for (isize i = 0; i < tmp_res.count; i++)
  {
    Complex c = tmp_res.data[i];
    c.r = c.r * 1.0 / N;
    Sequence_add(perm_arena, &res, c);
  }

  return res;
}

Sequence fft_fft(Arena* perm_arena, Arena* tmp_arena, Sequence input)
{
  Sequence tmp_res = coley_turkey(input, tmp_arena, input.count, -1.0);

  Sequence res = Sequence_clone(perm_arena, tmp_res);
  for (i32 i = 0; i < res.count; i++)
  {
    res.data[i] = c_mul(res.data[i], (Complex) { 1.0 / input.count, 0.0 });
  }
  return res;
}


// see https://github.com/0xb01u/pyFFT/blob/master/Cooley-Tukey.py
// and https://github.com/bubnicbf/Fast-Fourier-Transform-using-Cooley-Tukey-algorithm/blob/master/FFT.cpp  
//coley turkey fft  
Sequence coley_turkey(Sequence input, Arena* tmp_arena, isize N, f64 exp_sign)
{
  // TODO MAKE SURE IT IS 0 initialized
  Sequence output = Sequence_empty(tmp_arena, N);

  memset(output.data, 0, output.capacity * sizeof(Complex));
  output.count = N;
  if (N == 1)
  {
    output.data[0] = input.data[0];
    return output;
  }

  // divide and conquer

  // assume N is power of 2
  Sequence input_even = Sequence_empty(tmp_arena, N / 2);
  Sequence input_odd = Sequence_empty(tmp_arena, N / 2);
  for (i32 i = 0; i < N / 2; i++)
  {
    Sequence_add(tmp_arena, &input_even, input.data[i * 2]);
    Sequence_add(tmp_arena, &input_odd, input.data[i * 2 + 1]);
  }

  Sequence even = coley_turkey(input_even, tmp_arena, N / 2, exp_sign);
  Sequence odd = coley_turkey(input_odd, tmp_arena, N / 2, exp_sign);


  // combine
  for (isize k = 0; k < N / 2; k++)
  {
    Complex w = { 0 };
    double angle = 2 * PI * k / N;
    w.r = cos(angle);
    w.i = exp_sign * sin(angle);
    output.data[k] = c_add(even.data[k], c_mul(odd.data[k], w));
    output.data[k + N / 2] = c_sub(even.data[k], c_mul(odd.data[k], w));
  }

  return output;

}
