#pragma once
#include "pch.h"
#include "Vector.h"

namespace cml {
	float lerp(const float a, const float b, const float t);
	cml::vec3 lerp(const cml::vec3& a, const cml::vec3& b, const float t);
	float radian(const float degree);
}