#pragma once
#include "pch.h"
#include "RenderSettings.h"
#include "RenderTarget.h"
#include "Mesh.h"
#include "Shading.h"
#include "VertexShader.h"

struct BoundingBox2DInt {
	inline BoundingBox2DInt(cml::ivec2 min, cml::ivec2 max) : min(min), max(max) {};
	cml::ivec2 min;
	cml::ivec2 max;
};

void rasterize(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings);

std::vector<Triangle> clipTriangles(std::vector<Triangle> in);
std::vector<Vertex> clipLineAgainsAxisAlignedLine(const std::vector<Vertex>& vertices, const int& clipCoordinate, const float clipLimit, const bool& lowerLimit);

int edgeFunction(const cml::ivec2& v, const cml::ivec2& deltaV, const cml::ivec2& p);
bool isBarycentricValid(const cml::ivec3 b);
bool isBarycentricVaildSingleSIMD(const __m128i& b);
__m128i isBarycentricValidMultipleSIMD(const __m128i& x, const __m128i& y, const __m128i& z);

cml::ivec2 coordinateNDCtoRaster(const cml::vec3& p, const uint32_t& width, const uint32_t& height);
Vertex interpolateVertexData(const Triangle& t, const cml::vec3 b);

InterpolationResult interpolateVertexDataSIMD(const Triangle& t, const __m128& bX, const __m128& bY, const __m128& bZ);