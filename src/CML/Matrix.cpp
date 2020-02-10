#include "pch.h"
#include "Matrix.h"

namespace cml {
	mat4x4::mat4x4() {
		data[0] = cml::vec4(1.f, 0.f, 0.f, 0.f);
		data[1] = cml::vec4(0.f, 1.f, 0.f, 0.f);
		data[2] = cml::vec4(0.f, 0.f, 1.f, 0.f);
		data[3] = cml::vec4(0.f, 0.f, 0.f, 1.f);
	}

	cml::vec4& mat4x4::operator [] (const int& rowIndex) {
		return data[rowIndex];
	}

	cml::vec4 matrixMult (cml::vec4 v, cml::mat4x4 m) {
		cml::vec4 res;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				res[i] += m[i][j] * v[j];
			}
		}
		return res;
	}
}