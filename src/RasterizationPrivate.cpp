#include "pch.h"
#include "RasterizationPrivate.h"

//multithreaded tiled rasterization
//void rasterizeBackup(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings) {
//
//	//transform triangles to NDC
//	std::vector<std::array<cml::ivec2, 3>> triangleNDC;
//	triangleNDC.reserve(triangles.size());
//	
//	for (const auto& t : triangles) {
//		std::array<cml::ivec2, 3> NDC = {
//			coordinateNDCtoRaster(t.v1.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight()),
//			coordinateNDCtoRaster(t.v2.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight()),
//			coordinateNDCtoRaster(t.v3.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight())};
//		triangleNDC.push_back(NDC);
//	}
//
//	//store triangle indices per tile
//	cml::ivec2 tileSize = cml::ivec2(16, 16);
//	cml::ivec2 targetResRounded = cml::ivec2(target.getWidth(), target.getHeight());
//	targetResRounded.x += tileSize.x - targetResRounded.x % tileSize.x;
//	targetResRounded.y += tileSize.y - targetResRounded.y % tileSize.y;
//
//	size_t tileRowSize = targetResRounded.x / tileSize.x;
//	size_t tileColumnSize = targetResRounded.y / tileSize.y;
//
//	assert(targetResRounded.x % tileRowSize == 0);
//	assert(targetResRounded.y % tileColumnSize == 0);
//
//	size_t nTiles = tileRowSize * tileColumnSize;
//	std::vector<std::vector<size_t>> tileTriangleList;
//	tileTriangleList.resize(nTiles);
//
//	auto tileIndexFromXY = [tileRowSize](size_t x, size_t y) {
//		return tileRowSize * y + x;
//	};
//
//
//	for (size_t iTriangle = 0; iTriangle < triangles.size(); iTriangle++) {
//
//		std::array<cml::ivec2, 3> NDC = triangleNDC[iTriangle];
//		BoundingBox2DInt triangleBB(
//			cml::ivec2(std::min(std::min(NDC[0].x, NDC[1].x), NDC[2].x), std::min(std::min(NDC[0].y, NDC[1].y), NDC[2].y)),
//			cml::ivec2(std::max(std::max(NDC[0].x, NDC[1].x), NDC[2].x), std::max(std::max(NDC[0].y, NDC[1].y), NDC[2].y))
//		);
//
//		//round to tile sizes
//		triangleBB.min.x -= triangleBB.min.x % tileSize.x;
//		triangleBB.min.y -= triangleBB.min.y % tileSize.y;
//
//		triangleBB.max.x += triangleBB.max.x % tileSize.x;
//		triangleBB.max.y += triangleBB.max.y % tileSize.y;
//
//		//write indices to tiles
//		for (size_t x = triangleBB.min.x; x < triangleBB.max.x; x += tileSize.x) {
//			for (size_t y = triangleBB.min.y; y < triangleBB.max.y; y += tileSize.y) {
//				cml::ivec2 tileCo = cml::ivec2(x / tileSize.x, y / tileSize.y);
//				size_t tileIndex = tileIndexFromXY(tileCo.x, tileCo.y);
//				tileTriangleList[tileIndex].push_back(iTriangle);
//			}
//		}
//	}
//
//	//tiles are assigned to threads in an alternating fashion
//	//threads are started after task lists are complete
//	//avoids mutex, at the cost of dynamic thread allocation
//	//slightly faster than threadpool
//	const unsigned int nThreads = std::thread::hardware_concurrency();
//	std::vector<std::vector<std::function<void()>>> tasksPerThread;
//	tasksPerThread.resize(nThreads);
//
//	size_t currentThread = 0;
//	for (uint32_t tileX = 0; tileX < tileRowSize; tileX++) {
//		for (uint32_t tileY = 0; tileY < tileColumnSize; tileY++) {
//			size_t tileIndex = tileIndexFromXY(tileX, tileY);
//			const auto& indexList = tileTriangleList[tileIndex];
//			for (const size_t triangleIndex : indexList) {
//				const Triangle t = triangles[triangleIndex];
//				const auto NDC = triangleNDC[triangleIndex];
//
//				cml::ivec2 bbMin = cml::ivec2(tileSize.x * tileX, tileSize.y * tileY);
//				cml::ivec2 bbMax = bbMin + tileSize;
//				const BoundingBox2DInt tileBB = BoundingBox2DInt(bbMin, bbMax);
//
//				auto execution = [&triangles, &target, &settings, t, tileBB, NDC]() {
//					for (int x = tileBB.min.x; x < tileBB.max.x; x++) {
//						for (int y = tileBB.min.y; y < tileBB.max.y; y++) {
//							cml::vec3 b = calcBarycentric(NDC[0], NDC[1], NDC[2], cml::ivec2((int)x, (int)y));
//							if (isBarycentricInvalid(b.x) || isBarycentricInvalid(b.y) || isBarycentricInvalid(b.z)) {
//								continue;
//							}
//							Vertex v = interpolateVertexData(t, b);
//							int depth = (int)(-v.position.z * INT_MAX);
//							target.writeDepthTest((size_t)x, (size_t)y, settings.shadingFunction(v, settings.shaderInput), depth);
//						}
//					}
//				};
//
//				tasksPerThread[currentThread].push_back(execution);
//				currentThread++;
//				currentThread = currentThread % nThreads;
//			}
//		}
//	}
//	
//	//create threads
//	std::vector<std::thread> threads;
//	threads.resize(nThreads);
//
//	for (size_t threadIndex = 0; threadIndex < nThreads; threadIndex++) {
//		threads[threadIndex] = std::thread([&tasksPerThread, threadIndex]() {
//			for (const auto& task : tasksPerThread[threadIndex]) {
//				task();
//			}
//		});
//	}
//
//	//wait until work is finished
//	for (size_t threadIndex = 0; threadIndex < nThreads; threadIndex++) {
//		threads[threadIndex].join();
//	}
//}

void rasterize(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings) {

	for (const auto& t : triangles) {

		cml::ivec2 v0 = coordinateNDCtoRaster(t.v1.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight());
		cml::ivec2 v1 = coordinateNDCtoRaster(t.v2.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight());
		cml::ivec2 v2 = coordinateNDCtoRaster(t.v3.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight());

		cml::ivec2 edgeVectors[3] = {
			v0 - v2,
			v1 - v0,
			v2 - v1
		};

		float area = (float)edgeFunction(v0, edgeVectors[0], v1);
		//skip no surface + backface culling
		if (area == 0.f) {
			continue;
		}

		cml::ivec2 bbMin = cml::ivec2(std::min(std::min(v0.x, v1.x), v2.x), std::min(std::min(v0.y, v1.y), v2.y));
		cml::ivec2 bbMax = cml::ivec2(std::max(std::max(v0.x, v1.x), v2.x), std::max(std::max(v0.y, v1.y), v2.y));
		const BoundingBox2DInt tileBB = BoundingBox2DInt(bbMin, bbMax);

		cml::vec3 initialB;
		initialB.y = (float)edgeFunction(v0, edgeVectors[0], tileBB.min);
		initialB.z = (float)edgeFunction(v1, edgeVectors[1], tileBB.min);
		initialB.x = (float)edgeFunction(v2, edgeVectors[2], tileBB.min);

		cml::vec3 b = initialB;

		int columnCounter = 0;
		for (int x = tileBB.min.x; x < tileBB.max.x; x++) {
			for (int y = tileBB.min.y; y < tileBB.max.y; y++) {				

				if (isBarycentricValid(b.x) && isBarycentricValid(b.y) && isBarycentricValid(b.z)) {
					const cml::vec3 bNormalized = b / area;
					Vertex v = interpolateVertexData(t, bNormalized);
					int depth = (int)(-v.position.z * INT_MAX);
					target.writeDepthTest((size_t)x, (size_t)y, settings.shadingFunction(v, settings.shaderInput), depth);
				}
				b.y -= edgeVectors[0].x;
				b.z -= edgeVectors[1].x;
				b.x -= edgeVectors[2].x;
			}
			//reset
			b = initialB;
			//add n * x
			columnCounter++;
			b.y += edgeVectors[0].y * columnCounter;
			b.z += edgeVectors[1].y * columnCounter;
			b.x += edgeVectors[2].y * columnCounter;
		}
	}
}

int edgeFunction(const cml::ivec2& v, const cml::ivec2& deltaV, const cml::ivec2& p) {
	return (p.x - v.x) * deltaV.y - (p.y - v.y) * deltaV.x;
};

bool isBarycentricValid(const float value) {
	return value <= 0.f;
}

std::vector<Triangle> clipTriangles(std::vector<Triangle> in) {
	std::vector<Triangle> result;
	for (size_t triangleI = 0; triangleI < in.size(); triangleI++) {
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
			for (size_t secondPointIndex = 1; secondPointIndex < pointsClipped.size() - 1; secondPointIndex++) {
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

std::vector<Vertex> clipLineAgainsAxisAlignedLine(const std::vector<Vertex>& vertices, const int& clipCoordinate, const float clipLimit, const bool& lowerLimit) {
	std::vector<Vertex> verticesClipped;

	std::function<bool(float, float)> isInside = [](const float p, const float l) {
		return p <= l;
	};

	std::function<bool(float, float)> isOutside = [](const float p, const float l) {
		return p > l;
	};

	if (lowerLimit) {
		isInside = [](const float p, const float l) {
			return p >= l;
		};
		isOutside = [](const float p, const float l) {
			return p < l;
		};
	}

	for (size_t pointI = 0; pointI < vertices.size(); pointI++) {
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

cml::ivec2 coordinateNDCtoRaster(const cml::vec3& p, const uint32_t& width, const uint32_t& height) {
	const cml::vec3 pZeroToOne = p * 0.5f + 0.5f;
	return cml::ivec2((int)std::round(pZeroToOne.x * (width - 1)), (int)std::round(pZeroToOne.y * (height - 1)));
}

Vertex interpolateVertexData(const Triangle& t, const cml::vec3 b) {
	Vertex res;
	res.position = t.v1.position * b.x + t.v2.position * b.y + t.v3.position * b.z;
	res.normal = t.v1.normal * b.x + t.v2.normal * b.y + t.v3.normal * b.z;
	res.uv = t.v1.uv * b.x + t.v2.uv * b.y + t.v3.uv * b.z;
	return res;
}