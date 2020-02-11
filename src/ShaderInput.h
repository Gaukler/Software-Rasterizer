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
