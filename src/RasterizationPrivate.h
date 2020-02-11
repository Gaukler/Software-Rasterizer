#pragma once
#include "pch.h"
#include "RenderSettings.h"
#include "RenderTarget.h"
#include "Mesh.h"
#include "Shading.h"
#include "VertexShader.h"

void rasterizeLines(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);
void rasterizePoints(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);
void rasterizeFill(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);

std::vector<Triangle> clipTriangles(std::vector<Triangle> in);
std::vector<Vertex> clipLineAgainsAxisAlignedLine(const std::vector<Vertex>& vertices, const int& clipCoordinate, const float clipLimit, const bool& lowerLimit);

cml::vec3 calcBarycentric(const cml::ivec2& v0, const cml::ivec2& v1, const cml::ivec2& v2, const cml::ivec2& p);
bool isBarycentricInvalid(const float value);

cml::ivec2 coordinateNDCtoRaster(const cml::vec3& p, const uint32_t& width, const uint32_t& height);
Vertex interpolateVertexData(const Triangle& t, const cml::vec3 b);
