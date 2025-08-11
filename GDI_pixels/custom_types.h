#pragma once
#include "stdint.h"

typedef uint8_t   u8;
typedef wchar_t  c16;
typedef int32_t   b32;
typedef int32_t   i32;
typedef int64_t   i64;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef float     f32;
typedef double    f64;
typedef uintptr_t uptr;
typedef ptrdiff_t size;
typedef size_t    usize;


#define sizeof(x)    (ptrdiff_t)sizeof(x)
#define countof(a)   (sizeof(a) / sizeof(*(a)))
#define lengthof(s)  (countof(s) - 1)