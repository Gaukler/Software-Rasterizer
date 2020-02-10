#pragma once
#include "pch.h"
#include "Matrix.h"

namespace cml {
	mat4x4 projectionMatrix(float near, float far, float fovY, float aspectRatio = 1.f);
	mat4x4 orthographicMatrix(float near, float far, float aspectRatio = 1.f, float t = 1.f);
	mat4x4 translationMatrix(cml::vec3 t);
	mat4x4 viewMatrixLookAt(vec3 position, const vec3& lookAt);
	mat4x4 uniformScaleMatrix(const float& scale);
}