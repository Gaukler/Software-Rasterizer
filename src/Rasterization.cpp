#include "pch.h"
#include "Rasterization.h"

void drawTriangles(const std::vector<Triangle>& triangleList, RenderTarget& target, const RenderSettings& settings) {

	std::vector<Triangle> transformedTriangles;
	transformedTriangles.reserve(triangleList.size());
	for (int iTriangle = 0; iTriangle < triangleList.size(); iTriangle++) {
		Triangle t;
		t.v1 = settings.vertexFunctions(triangleList[iTriangle].v1, settings.shaderInput);
		t.v2 = settings.vertexFunctions(triangleList[iTriangle].v2, settings.shaderInput);
		t.v3 = settings.vertexFunctions(triangleList[iTriangle].v3, settings.shaderInput);
		transformedTriangles.push_back(t);
	}

	std::vector<Triangle> clipedTriangles = clipTriangles(transformedTriangles);

	if (settings.rasterType == RasterizationType::Fill) {
		rasterizeFill(clipedTriangles, target, settings);
	}
	else if (settings.rasterType == RasterizationType::Line) {
		rasterizeLines(clipedTriangles, target, settings);
	}
	else if (settings.rasterType == RasterizationType::Point) {
		rasterizePoints(clipedTriangles, target, settings);
	}
}
	

void swapPoints(cml::ivec2& i1, cml::ivec2& i2) {
	cml::ivec2 temp = i1;
	i1 = i2;
	i2 = temp;
}

float lineFunction(const cml::ivec2& v0, const cml::ivec2& v1, const cml::ivec2& p) {
	return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
}

cml::vec3 calcBarycentric(const cml::ivec2& v0, const cml::ivec2& v1, const cml::ivec2& v2, const cml::ivec2& p) {
	cml::vec3 result;
	result.z = lineFunction(v0, v1, p);
	result.x = lineFunction(v1, v2, p);
	result.y = lineFunction(v2, v0, p);
	float d = lineFunction(v0, v1, v2);
	cml::vec3 test = result / d;
	return test;
}

cml::ivec2 coordinateNDCtoRaster(cml::vec3 p, const int& width, const int& height) {
	p = p * 0.5f + 0.5f;
	return cml::ivec2(std::round(p.x * (width - 1)), std::round(p.y * (height - 1)));
}

bool isBarycentricInvalid(float value) {
	return (value < 0.f) || (value > 1.f);
}

bool isBarycentricInvalidLine(cml::vec3 b) {
	const float delta = 0.02f;
	return !((abs(b.x) < delta) || (abs(b.y) < delta) || (abs(b.z) < delta));
}

//this is not efficient, but simple and allows for variable line thickness
void rasterizeLines(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings) {
	for (int iTriangle = 0; iTriangle < triangles.size(); iTriangle++) {
		Triangle t = triangles[iTriangle];

		const cml::ivec2 p1 = coordinateNDCtoRaster(t.v1.position, target.getWidth(), target.getHeight());
		const cml::ivec2 p2 = coordinateNDCtoRaster(t.v2.position, target.getWidth(), target.getHeight());
		const cml::ivec2 p3 = coordinateNDCtoRaster(t.v3.position, target.getWidth(), target.getHeight());

		cml::ivec2 bb_min = cml::ivec2(std::min(std::min(p1.x, p2.x), p3.x), std::min(std::min(p1.y, p2.y), p3.y));
		cml::ivec2 bb_max = cml::ivec2(std::max(std::max(p1.x, p2.x), p3.x), std::max(std::max(p1.y, p2.y), p3.y));

		for (int x = bb_min.x; x < bb_max.x; x++) {
			for (int y = bb_min.y; y < bb_max.y; y++) {
				cml::vec3 b = calcBarycentric(p1, p2, p3, cml::ivec2(x, y));
				if (isBarycentricInvalid(b.x) || isBarycentricInvalid(b.y) || isBarycentricInvalid(b.z) || isBarycentricInvalidLine(b)) {
					continue;
				}
				cml::vec3 position = t.v1.position * b.x + t.v2.position * b.y + t.v3.position * b.z;
				int depth = -position.z * INT_MAX;
				target.writeDepthTest(x, y, settings.shadingFunction(t, b, settings.shaderInput), depth);
			}
		}
	}
}

bool isInsideUpperLimit (float p, float l) {
	return p <= l;
};
bool isOutsideUpperLimit(float p, float l) {
	return p > l;
};

bool isInsideLowerLimit(float p, float l) {
	return p >= l;
};
bool isOutsideLowerLimit(float p, float l) {
	return p < l;
};

std::vector<Vertex> clipLineAgainsAxisAlignedLine(const std::vector<Vertex>& vertices, const int& clipCoordinate, const float clipLimit, const bool& lowerLimit) {
	std::vector<Vertex> verticesClipped;

	auto isInside = &isInsideUpperLimit;
	auto isOutside = &isOutsideUpperLimit;
	if (lowerLimit) {
		isInside = &isInsideLowerLimit;
		isOutside = &isOutsideLowerLimit;
	}

	for (int pointI = 0; pointI < vertices.size(); pointI++) {
		Vertex v1 = vertices[pointI];
		Vertex v2 = vertices[(pointI + 1) % vertices.size()];

		if (isInside(v1.position[clipCoordinate], clipLimit) && isInside(v2.position[clipCoordinate], clipLimit)) { //both points in
			verticesClipped.push_back(v2);
		}
		else if (isOutside(v1.position[clipCoordinate], clipLimit) && isOutside(v2.position[clipCoordinate], clipLimit)) { //both points out
			//clip both
		}
		else { //mixed: clip
			float t = (clipLimit - v1.position[clipCoordinate]) / (v2.position[clipCoordinate] - v1.position[clipCoordinate]);
			if (t != 0 && t != 1) {
				Vertex newVertex;
				newVertex.position = v1.position + (v2.position - v1.position) * t;
				newVertex.normal = v1.normal + (v2.normal - v1.normal) * t;
				newVertex.uv = v1.uv + (v2.uv - v1.uv) * t;
				verticesClipped.push_back(newVertex);
			}
			if (isInside(v2.position[clipCoordinate], clipLimit)) {
				verticesClipped.push_back(v2);
			}
		}
	}
	return verticesClipped;
}

std::vector<Triangle> clipTriangles(std::vector<Triangle> in) {
	std::vector<Triangle> result;
	for (int triangleI = 0; triangleI < in.size(); triangleI++) {
		std::vector<Vertex> points = { in[triangleI].v1, in[triangleI].v2, in[triangleI].v3 };
		std::vector<Vertex> pointsClipped;
		
		pointsClipped = clipLineAgainsAxisAlignedLine(points, 1, 1.f, false);		 //clip top
		pointsClipped = clipLineAgainsAxisAlignedLine(pointsClipped, 0, 1.f, false); //clip right
		pointsClipped = clipLineAgainsAxisAlignedLine(pointsClipped, 1, -1.f, true); //clip bot
		pointsClipped = clipLineAgainsAxisAlignedLine(pointsClipped, 0, -1.f, true); //clip left

		pointsClipped = clipLineAgainsAxisAlignedLine(pointsClipped, 2, -1.f, true); //clip back
		pointsClipped = clipLineAgainsAxisAlignedLine(pointsClipped, 2, 1.f, false); //clip forward

		//triangle fan
		if (pointsClipped.size() >= 3) {
			Vertex v1 = pointsClipped[0];
			for (int secondPointIndex = 1; secondPointIndex < pointsClipped.size() - 1; secondPointIndex++) {
				Vertex v2 = pointsClipped[secondPointIndex];
				Vertex v3 = pointsClipped[secondPointIndex + 1];
				Triangle t;
				t.v1 = v1;
				t.v2 = v2;
				t.v3 = v3;
				result.push_back(t);
			}
		}
	}
	return result;
}

void rasterizePoints(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings) {
	for (int i = 0; i < triangles.size(); i++) {

		cml::ivec2 position = coordinateNDCtoRaster(triangles[i].v1.position, target.getWidth(), target.getHeight());
		int depth = -triangles[i].v1.position.z * INT_MAX;
		target.writeDepthTest(position.x, position.y, settings.shadingFunction(triangles[i], cml::vec3(1.f, 0.f, 0.f), settings.shaderInput), depth);

		position = coordinateNDCtoRaster(triangles[i].v2.position, target.getWidth(), target.getHeight());
		depth = -triangles[i].v2.position.z * INT_MAX;
		target.writeDepthTest(position.x, position.y, settings.shadingFunction(triangles[i], cml::vec3(0.f, 1.f, 0.f), settings.shaderInput), depth);

		position = coordinateNDCtoRaster(triangles[i].v3.position, target.getWidth(), target.getHeight());
		depth = -triangles[i].v3.position.z * INT_MAX;
		target.writeDepthTest(position.x, position.y, settings.shadingFunction(triangles[i], cml::vec3(0.f, 0.f, 1.f), settings.shaderInput), depth);
	}
}

void rasterizeFill(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings) {
	for (int iTriangle = 0; iTriangle < triangles.size(); iTriangle++) {
		const Triangle t = triangles[iTriangle];

		const cml::ivec2 p1 = coordinateNDCtoRaster(t.v1.position, target.getWidth(), target.getHeight());
		const cml::ivec2 p2 = coordinateNDCtoRaster(t.v2.position, target.getWidth(), target.getHeight());
		const cml::ivec2 p3 = coordinateNDCtoRaster(t.v3.position, target.getWidth(), target.getHeight());

		cml::ivec2 bb_min = cml::ivec2(std::min(std::min(p1.x, p2.x), p3.x), std::min(std::min(p1.y, p2.y), p3.y));
		cml::ivec2 bb_max = cml::ivec2(std::max(std::max(p1.x, p2.x), p3.x), std::max(std::max(p1.y, p2.y), p3.y));

		for (int x = bb_min.x; x < bb_max.x; x++) {
			for (int y = bb_min.y; y < bb_max.y; y++) {
				cml::vec3 b = calcBarycentric(p1, p2, p3, cml::ivec2(x, y));
				if (isBarycentricInvalid(b.x) || isBarycentricInvalid(b.y) || isBarycentricInvalid(b.z)) {
					continue;
				}
				cml::vec3 position = t.v1.position * b.x + t.v2.position * b.y + t.v3.position * b.z;
				int depth = -position.z * INT_MAX;
				target.writeDepthTest(x, y, settings.shadingFunction(t, b, settings.shaderInput), depth);
			}
		}
	}
}