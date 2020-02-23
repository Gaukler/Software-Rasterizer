#pragma once
#include "pch.h"
#include "Shading.h"
#include "VertexShader.h"

struct RenderSettings {
	inline RenderSettings(cml::vec3(*shadingFunction)(const Vertex& t, const ShaderInput& input), VertexFunctions::VertexShaderResult(*vertexFunctions)(const Vertex& v, const ShaderInput& input),
		ShaderInput shaderInput)
		: shadingFunction(shadingFunction), vertexFunctions(vertexFunctions), shaderInput(shaderInput) {};

	cml::vec3(*shadingFunction)(const Vertex& t, const ShaderInput& input);
	VertexFunctions::VertexShaderResult(*vertexFunctions)(const Vertex& v, const ShaderInput& input);

	ShaderInput shaderInput;
private:
	float padding = 0.f;
};
