#pragma once
#include "pch.h"
#include "Image.h"
#include "CML/Matrix.h"

struct ShaderInput {
	inline ShaderInput(RGBImage* texture, const cml::vec3 lightPosition, const cml::mat4x4 projectionMatrix, const cml::mat4x4 viewMatrix)
		: texture(texture), lightPosition(lightPosition), projectionMatrix(projectionMatrix), viewMatrix(viewMatrix) {};
	RGBImage* texture;
	cml::vec3 lightPosition;
	mutable cml::mat4x4 projectionMatrix;
	mutable cml::mat4x4 viewMatrix;
private:
	float padding = 0.f;
};

//SIMD result of four vertex interpolations
struct InterpolationResult {
	union { __m128 normalX; float normalXArr[4]; };
	union { __m128 normalY; float normalYArr[4]; };
	union { __m128 normalZ; float normalZArr[4]; };

	union { __m128 posX;	float posXArr[4]; };
	union { __m128 posY;	float posYArr[4]; };
	union { __m128 posZ; float posZArr[4]; };

	union { __m128 uvX; float uvXArr[4]; };
	union { __m128 uvY; float uvYArr[4]; };
};