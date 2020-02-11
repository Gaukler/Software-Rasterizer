#pragma once
#include "pch.h"
#include "Shading.h"
#include "VertexShader.h"

enum class RasterizationType { Point, Line, Fill };

struct RenderSettings {
	inline RenderSettings(cml::vec3(*shadingFunction)(const Vertex& t, const ShaderInput& input), VertexFunctions::VertexShaderResult(*vertexFunctions)(const Vertex& v, const ShaderInput& input),
		ShaderInput shaderInput, RasterizationType rasterType)
		: shadingFunction(shadingFunction), vertexFunctions(vertexFunctions), shaderInput(shaderInput), rasterType(rasterType) {};

	cml::vec3(*shadingFunction)(const Vertex& t, const ShaderInput& input);
	VertexFunctions::VertexShaderResult(*vertexFunctions)(const Vertex& v, const ShaderInput& input);

	ShaderInput shaderInput;
	RasterizationType rasterType;
private:
	float padding = 0.f;
};
