#pragma once
#include "custom_types.h"


void a_assert(b32 expression, char* expr_str, char* file, i32 line);


#define ASSERT(expr) a_assert(expr, #expr, __FILE__, __LINE__)
