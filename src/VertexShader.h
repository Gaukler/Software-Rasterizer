#pragma once
#include "pch.h"
#include "CML/Vector.h"
#include "Mesh.h"
#include "ShaderInput.h"

namespace VertexFunctions {
	struct VertexShaderResult {
		float positionW = 0.f;
		Vertex vertex;
	};

	VertexShaderResult passTrough(const Vertex& vIn, const ShaderInput& input);
	VertexShaderResult viewProjection(const Vertex& vIn, const ShaderInput& input);
}