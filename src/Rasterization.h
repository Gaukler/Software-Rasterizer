#pragma once
#include "pch.h"
#include "RenderTarget.h"
#include "Mesh.h"
#include "Shading.h"
#include "VertexShader.h"

enum class RasterizationType { Point, Line, Fill };

struct RenderSettings {
	cml::vec3(*shadingFunction)(const Vertex& t, const ShaderInput& input);
	VertexFunctions::VertexShaderResult(*vertexFunctions)(const Vertex& v, const ShaderInput& input);
	ShaderInput shaderInput;
	RasterizationType rasterType;
private:
	float padding = 0.f;
};

std::vector<Triangle> clipTriangles(std::vector<Triangle> in);
void drawTriangles(const std::vector<Triangle>& triangleList, RenderTarget& target, const RenderSettings& settings);
void rasterizeLines(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);
void rasterizePoints(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);
void rasterizeFill(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);