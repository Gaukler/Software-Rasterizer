#pragma once
#include "../pch.h"
#include "Vector.h"

namespace cml {

	struct mat4x4 {
		mat4x4();
		cml::vec4 data[4];
		cml::vec4& operator [] (const int& rowIndex);
	};

	cml::vec4 matrixMult(cml::vec4 v, cml::mat4x4 m);
}