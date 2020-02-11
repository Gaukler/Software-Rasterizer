#pragma once
#include "pch.h"
#include "Matrix.h"

namespace cml {
	mat4x4 projectionMatrix(const float near, const float far, const float fovY, const float aspectRatio = 1.f);
	mat4x4 orthographicMatrix(const float near, const float far, const float aspectRatio = 1.f, const float t = 1.f);
	mat4x4 translationMatrix(const cml::vec3& t);
	mat4x4 viewMatrixLookAt(const vec3& position, const vec3& lookAt);
	mat4x4 uniformScaleMatrix(const float scale);
}