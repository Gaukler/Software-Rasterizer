#include "pch.h"
#include "Common.h"

namespace cml {
	float lerp(float a, float b, float t) {
		return (1.f - t) * a + t * b;
	}

	cml::vec3 lerp(cml::vec3 a, cml::vec3 b, float t) {
		return a * (1.f - t) + b * t;
	}

	float radian(const float& degree) {
		return degree / 180.f * 3.1415f;
	}
}