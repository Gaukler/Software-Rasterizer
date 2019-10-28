#include "pch.h"
#include "VertexShader.h"

namespace VertexFunctions {
	VertexShaderResult passTrough(const Vertex& vIn, const ShaderInput& input) {
		VertexShaderResult result;
		result.positionW = 1.f;
		result.vertex = vIn;
		return result;
	}

	VertexShaderResult viewProjection(const Vertex& vIn, const ShaderInput& input) {
		VertexShaderResult result;
		result.vertex = vIn;
		cml::vec4 viewPos = cml::matrixMult(cml::vec4(result.vertex.position, 1.f), input.viewMatrix);
		cml::vec4 pos = cml::matrixMult(viewPos, input.projectionMatrix);
		result.vertex.position.x = pos.x;
		result.vertex.position.y = pos.y;
		result.vertex.position.z = pos.z;
		result.positionW = pos.w;

		return result;
	}
}