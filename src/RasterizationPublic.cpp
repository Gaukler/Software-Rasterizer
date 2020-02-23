#include "pch.h"
#include "RasterizationPublic.h"
#include "RasterizationPrivate.h"

void drawTriangles(const std::vector<Triangle>& triangleList, RenderTarget& target, const RenderSettings& settings) {

	std::vector<Triangle> transformedTriangles;
	transformedTriangles.reserve(triangleList.size());
	for (size_t iTriangle = 0; iTriangle < triangleList.size(); iTriangle++) {
		Triangle t;
		VertexFunctions::VertexShaderResult vResult;

		vResult = settings.vertexFunctions(triangleList[iTriangle].v1, settings.shaderInput);
		t.v1 = vResult.vertex;
		t.v1.position = t.v1.position / vResult.positionW;

		vResult = settings.vertexFunctions(triangleList[iTriangle].v2, settings.shaderInput);
		t.v2 = vResult.vertex;
		t.v2.position = t.v2.position / vResult.positionW;

		vResult = settings.vertexFunctions(triangleList[iTriangle].v3, settings.shaderInput);
		t.v3 = vResult.vertex;
		t.v3.position = t.v3.position / vResult.positionW;

		transformedTriangles.push_back(t);
	}

	std::vector<Triangle> clipedTriangles = clipTriangles(transformedTriangles);
	rasterize(clipedTriangles, target, settings);
}