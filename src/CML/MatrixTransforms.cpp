#include "pch.h"
#include "MatrixTransforms.h"

namespace cml {
	mat4x4 projectionMatrix(const float near, const float far, const float fovY, const float aspectRatio) {
		const float t = tan(fovY * 0.5f) * near;
		const float r = t * aspectRatio;
		mat4x4 m;
		m[0][0] = near / r;
		m[1][1] = near / t;
		m[2][2] = (far + near) / (far - near);
		m[2][3] = -2.f * far * near / (far - near);
		m[3][2] = 1.f;
		m[3][3] = 0.f;
		return m;
	}

	mat4x4 orthographicMatrix(const float near, const float far, const float aspectRatio, const float t) {
		const float r = t * aspectRatio;
		mat4x4 m;
		m[0][0] = near / r;
		m[1][1] = near / t;
		m[2][2] = 2.f / (far - near);
		m[2][3] = -(far + near) / (far - near);
		return m;
	}

	mat4x4 translationMatrix(const cml::vec3& t) {
		mat4x4 res;
		res[0][3] = t.x;
		res[1][3] = t.y;
		res[2][3] = t.z;
		return res;
	}

	mat4x4 viewMatrixLookAt(const vec3& position, const vec3& lookAt) {
		const vec3 forward = normalize(lookAt - position);
		vec3 up = vec3(0.f, 1.f, 0.f);
		const vec3 right = normalize(cross(forward, up));
		up = cross(right, forward);
		mat4x4 m;
		m[0] = vec4(right, 0.f);
		m[1] = vec4(up, 0.f);
		m[2] = vec4(forward, 0.f);

		vec4 viewPosition = cml::vec4(position * -1.f, 0.f);
		viewPosition = matrixMult(viewPosition, m);
		m[0][3] = viewPosition.x;
		m[1][3] = viewPosition.y;
		m[2][3] = viewPosition.z;

		return m;
	}

	mat4x4 uniformScaleMatrix(const float scale) {
		mat4x4 res;
		res[0][0] = scale;
		res[1][1] = scale;
		res[2][2] = scale;
		return res;
	}
}