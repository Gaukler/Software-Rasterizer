#pragma once
#include "pch.h"
#include "CML/Vector.h"
#include "Mesh.h"
#include "ShaderInput.h"

namespace VertexFunctions {
	Vertex passTrough(const Vertex& vIn, const ShaderInput& input);
	Vertex viewProjection(const Vertex& vIn, const ShaderInput& input);
}