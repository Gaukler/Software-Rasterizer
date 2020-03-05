#include "pch.h"
#include "Shading.h"

namespace ShadingFunctions {

	//disable warning for unused parameter "input"
	#ifdef _MSC_VER
	#pragma warning(disable: 4100)
	#endif

	ColorSIMD normalVis(const InterpolationResult& v, const ShaderInput& input) {
		ColorSIMD out;
		out.r = v.normalX;
		out.g = v.normalY;
		out.b = v.normalZ;
		return out;
	}

	ColorSIMD depthVis(const InterpolationResult& v, const ShaderInput& input) {
		ColorSIMD out;
		out.r = v.posZ;
		out.g = v.posZ;
		out.b = v.posZ;
		return out;
	}

	ColorSIMD texture(const InterpolationResult& v, const ShaderInput& input){
		return input.texture->sampleSIMD(v.uvX, v.uvY);
	}

	ColorSIMD texturedLit(const InterpolationResult& v, const ShaderInput& input) {
		cml::vec3 light = cml::normalize(input.lightPosition);

		__m128 lx = _mm_set1_ps(light.x);
		__m128 ly = _mm_set1_ps(light.y);
		__m128 lz = _mm_set1_ps(light.z);

		__m128 dot = _mm_mul_ps(lx, v.normalX);
		dot = _mm_add_ps(dot, _mm_mul_ps(ly, v.normalY));
		dot = _mm_add_ps(dot, _mm_mul_ps(lz, v.normalZ));
		__m128 min = _mm_set1_ps(0.2f);
		dot = _mm_max_ps(dot, min);

		ColorSIMD texture = input.texture->sampleSIMD(v.uvX, v.uvY);

		ColorSIMD out;
		out.r = _mm_mul_ps(dot, texture.r);
		out.g = _mm_mul_ps(dot, texture.g);
		out.b = _mm_mul_ps(dot, texture.b);
		return out;
	}

	//reenable warning
	#ifdef _MSC_VER
	#pragma warning(default: 4100)
	#endif
}