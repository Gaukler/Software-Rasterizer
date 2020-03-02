#pragma once
#include "pch.h"
#include "Shading.h"
#include "VertexShader.h"
#include "ColorSIMD.h"

struct RenderSettings {
	inline RenderSettings(ColorSIMD(*shadingFunction)(const InterpolationResult& t, const ShaderInput& input), VertexFunctions::VertexShaderResult(*vertexFunctions)(const Vertex& v, const ShaderInput& input),
		ShaderInput shaderInput)
		: shadingFunction(shadingFunction), vertexFunctions(vertexFunctions), shaderInput(shaderInput) {};

	ColorSIMD(*shadingFunction)(const InterpolationResult& t, const ShaderInput& input);
	VertexFunctions::VertexShaderResult(*vertexFunctions)(const Vertex& v, const ShaderInput& input);

	ShaderInput shaderInput;
private:
	float padding = 0.f;
};
