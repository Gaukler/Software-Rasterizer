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
		__m128 lightPosX = _mm_set1_ps(input.lightPosition.x);
		__m128 lightPosY = _mm_set1_ps(input.lightPosition.x);
		__m128 lightPosZ = _mm_set1_ps(input.lightPosition.x);

		__m128 toLightX = _mm_sub_ps(lightPosX, v.posX);
		__m128 toLightY = _mm_sub_ps(lightPosY, v.posY);
		__m128 toLightZ = _mm_sub_ps(lightPosZ, v.posZ);

		__m128 lightLength = _mm_sqrt_ps(_mm_add_ps(_mm_mul_ps(toLightX, toLightX), _mm_add_ps(_mm_mul_ps(toLightY, toLightY), _mm_mul_ps(toLightZ, toLightZ))));

		toLightX = _mm_div_ps(toLightX, lightLength);
		toLightY = _mm_div_ps(toLightY, lightLength);
		toLightZ = _mm_div_ps(toLightZ, lightLength);

		__m128 dot = _mm_mul_ps(toLightX, v.normalX);
		dot = _mm_add_ps(dot, _mm_mul_ps(toLightY, v.normalY));
		dot = _mm_add_ps(dot, _mm_mul_ps(toLightZ, v.normalZ));
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