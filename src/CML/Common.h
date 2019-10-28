#pragma once
#include "../pch.h"
#include "Vector.h"

namespace cml {
	float lerp(float a, float b, float t);
	cml::vec3 lerp(cml::vec3 a, cml::vec3 b, float t);
	float radian(const float& degree);
}