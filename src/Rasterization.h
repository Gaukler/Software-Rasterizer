#pragma once
#include "pch.h"
#include "RenderTarget.h"
#include "Mesh.h"
#include "Shading.h"

enum class RasterizationType { Point, Line, Fill };
struct RenderSettings {
	RasterizationType rasterType;
	cml::vec3(*shadingFunction)(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
	Vertex(*vertexFunctions)(const Vertex& v, const ShaderInput& input);
	ShaderInput shaderInput;
};

std::vector<Triangle> clipTriangles(std::vector<Triangle> in);
void drawTriangles(const std::vector<Triangle>& triangleList, RenderTarget& target, const RenderSettings& settings);
void rasterizeLines(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);
void rasterizePoints(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);
void rasterizeFill(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);