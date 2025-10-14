#pragma once
#include "generated.h"


Complex c_mul(Complex c1, Complex c2);
double c_mag(Complex c);
Complex c_add(Complex a, Complex b);
Complex c_sub(Complex a, Complex b);


Sequence fft_dft(Arena* a, Sequence input);
Sequence fft_dft_inv(Arena* a, Sequence input);

Sequence fft_fft(Arena* perm_arena, Arena* tmp_arena, Sequence input);
Sequence fft_ifft(Arena* perm_arena, Arena* tmp_arena, Sequence input);