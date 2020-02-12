#include "pch.h"
#include "Common.h"

namespace cml {
	float lerp(const float a, const float b, const float t) {
		return (1.f - t) * a + t * b;
	}

	cml::vec3 lerp(const cml::vec3& a, const cml::vec3& b, const float t) {
		return a * (1.f - t) + b * t;
	}

	float radian(const float degree) {
		return degree / 180.f * 3.1415f;
	}
}