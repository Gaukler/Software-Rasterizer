#include "pch.h"
#include "VertexShader.h"

namespace VertexFunctions {
	Vertex passTrough(const Vertex& vIn, const ShaderInput& input) {
		return vIn;
	}

	Vertex viewProjection(const Vertex& vIn, const ShaderInput& input) {
		Vertex v = vIn;
		cml::vec4 viewPos = cml::matrixMult(cml::vec4(v.position, 1.f), input.viewMatrix);
		cml::vec4 pos = cml::matrixMult(viewPos, input.projectionMatrix);
		v.position.x = pos.x;
		v.position.y = pos.y;
		v.position.z = pos.z;
		v.position = v.position / pos.w;

		return v;
	}
}