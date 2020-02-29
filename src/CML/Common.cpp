#include "pch.h"
#include "Common.h"

namespace cml {
	float lerp(const float a, const float b, const float t) {
		return (1.f - t) * a + t * b;
	}

	cml::vec3 lerp(const cml::vec3& a, const cml::vec3& b, const float t) {
		//return a * (1.f - t) + b * t;
		union { __m128 aReg; float aArr[4]; };
		union { __m128 bReg; float bArr[4]; };

		aReg = _mm_setr_ps(a.x, a.y, a.z, 0.f);
		bReg = _mm_setr_ps(b.x, b.y, b.z, 0.f);

		float tInv = 1.f - t;

		__m128 tReg    = _mm_set1_ps(t);;
		__m128 tInvReg = _mm_set_ps1(tInv);

		aReg = _mm_mul_ps(aReg, tInvReg);
		bReg = _mm_mul_ps(bReg, tReg);

		union { __m128 valueReg; float value[4]; };
		valueReg = _mm_add_ps(aReg, bReg);
		return cml::vec3(value[0], value[1], value[2]);
	}

	float radian(const float degree) {
		return degree / 180.f * 3.1415f;
	}
}