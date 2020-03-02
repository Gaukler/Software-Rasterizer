#pragma once
#include "pch.h"

struct ColorSIMD {
	union { __m128 r; float rArr[4]; };
	union { __m128 g; float gArr[4]; };
	union { __m128 b; float bArr[4]; };
};