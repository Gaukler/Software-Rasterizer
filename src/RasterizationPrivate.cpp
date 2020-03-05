#include "pch.h"
#include "RasterizationPrivate.h"

void rasterize(const std::vector<Triangle>& triangles, RenderTarget& target, const RenderSettings& settings) {
	//rasterization based on: "A Parallel Algorithm for Polygon Rasterization", Juan Pineda 1988
	//the edge function is evaluated once for the first pixel, the subsequent values are computed by incrementing
	//for traversal the scheme from figure 5. is used:
	//the triangle is traversed from a central line, down from the top pixel
	//every row is rasterizesd by going left and right until running out of the triangle
	//row traversal processes four pixels at a time, using SIMD
	//when going down if the line goes out of the triangle the triangle has to be searched to reposition the central line

	//transform triangles to NDC
	std::vector<std::array<cml::ivec2, 3>> triangleNDC;
	triangleNDC.reserve(triangles.size());

	for (const auto& t : triangles) {
		std::array<cml::ivec2, 3> NDC = {
			coordinateNDCtoRaster(t.v1.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight()),
			coordinateNDCtoRaster(t.v2.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight()),
			coordinateNDCtoRaster(t.v3.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight()) };
		triangleNDC.push_back(NDC);
	}

	//store triangle indices per tile
	cml::ivec2 tileSize = cml::ivec2(32);
	cml::ivec2 targetResRounded = cml::ivec2(target.getWidth(), target.getHeight());
	targetResRounded.x += tileSize.x - targetResRounded.x % tileSize.x;
	targetResRounded.y += tileSize.y - targetResRounded.y % tileSize.y;

	size_t tileRowSize = targetResRounded.x / tileSize.x;
	size_t tileColumnSize = targetResRounded.y / tileSize.y;

	assert(targetResRounded.x % tileRowSize == 0);
	assert(targetResRounded.y % tileColumnSize == 0);

	size_t nTiles = tileRowSize * tileColumnSize;
	std::vector<std::vector<size_t>> tileTriangleList;
	tileTriangleList.resize(nTiles);

	for (size_t iTriangle = 0; iTriangle < triangles.size(); iTriangle++) {
		std::array<cml::ivec2, 3> NDC = triangleNDC[iTriangle];
		BoundingBox2DInt triangleBB(
			cml::ivec2(std::min(std::min(NDC[0].x, NDC[1].x), NDC[2].x), std::min(std::min(NDC[0].y, NDC[1].y), NDC[2].y)),
			cml::ivec2(std::max(std::max(NDC[0].x, NDC[1].x), NDC[2].x), std::max(std::max(NDC[0].y, NDC[1].y), NDC[2].y))
		);

		//round to tile sizes
		triangleBB.min.x -= triangleBB.min.x % tileSize.x;
		triangleBB.min.y -= triangleBB.min.y % tileSize.y;

		triangleBB.max.x += triangleBB.max.x % tileSize.x;
		triangleBB.max.y += triangleBB.max.y % tileSize.y;

		//write indices to tiles
		for (size_t x = triangleBB.min.x; x < triangleBB.max.x; x += tileSize.x) {
			for (size_t y = triangleBB.min.y; y < triangleBB.max.y; y += tileSize.y) {
				cml::ivec2 tileCo = cml::ivec2(x / tileSize.x, y / tileSize.y);
				size_t tileIndex = tileRowSize * tileCo.y + tileCo.x;
				tileTriangleList[tileIndex].push_back(iTriangle);
			}
		}
	}

	//tiles are assigned to threads in an alternating fashion
	//threads are started after task lists are complete
	//avoids mutex, at the cost of dynamic thread allocation
	//slightly faster than threadpool
	const unsigned int nThreads = std::thread::hardware_concurrency();
	std::vector<std::vector<size_t>> tileIndicesPerThread;
	std::vector<size_t> trianglePerThread;

	tileIndicesPerThread.resize(nThreads);
	trianglePerThread.resize(nThreads);


	/*size_t currentThread = 0;
	for (uint32_t tileY = 0; tileY < tileColumnSize; tileY++) {
		for (uint32_t tileX = 0; tileX < tileRowSize; tileX++) {
			size_t tileIndex = tileIndexFromXY(tileX, tileY);
			tileIndicesPerThread[currentThread].push_back(tileIndex);

			currentThread++;
			currentThread = currentThread % nThreads;
		}
	}*/

	//try to balance load between threads
	for (size_t tileIndex = 0; tileIndex < nTiles; tileIndex++) {
		int minNTriangles = trianglePerThread[0];
		int threadIndexWithLeastTriangles = 0;
		for (size_t threadIndex = 1; threadIndex < nThreads; threadIndex++) {
			if (trianglePerThread[threadIndex] < minNTriangles) {
				minNTriangles = trianglePerThread[threadIndex];
				threadIndexWithLeastTriangles = threadIndex;
			}
		}
		tileIndicesPerThread[threadIndexWithLeastTriangles].push_back(tileIndex);
		trianglePerThread[threadIndexWithLeastTriangles] += tileTriangleList[tileIndex].size();
	}

	//create threads
	std::vector<std::thread> threads;
	threads.resize(nThreads);

	for (size_t threadIndex = 0; threadIndex < nThreads; threadIndex++) {
		threads[threadIndex] = std::thread([](RenderTarget& target, const RenderSettings& settings, const size_t threadIndex, 
			const std::vector<std::vector<size_t>> tileTriangleList, const std::vector<Triangle>& triangles, 
			const size_t tileRowSize, const size_t tileColumnSize, const cml::ivec2 tileSize, const std::vector<std::vector<size_t>>& tileIndicesPerThread) {
				for(const size_t tileIndex : tileIndicesPerThread[threadIndex])
					for (const size_t triangleIndex : tileTriangleList[tileIndex]) {
						const Triangle& t = triangles[triangleIndex];

						cml::ivec2 bbMin;
						bbMin.x = tileIndex % tileRowSize;
						bbMin.y = tileIndex / tileRowSize;

						bbMin.x *= tileSize.x;
						bbMin.y *= tileSize.y;

						cml::ivec2 bbMax = bbMin + tileSize -cml::ivec2(1);

						rasterizeTriangleInBoundingBox(target, settings, t, bbMin, bbMax);
					}
		}, std::ref(target), std::ref(settings), threadIndex, std::ref(tileTriangleList), std::ref(triangles), tileRowSize, tileColumnSize, tileSize, std::ref(tileIndicesPerThread));
	}

	//wait until work is finished
	for (size_t threadIndex = 0; threadIndex < nThreads; threadIndex++) {
		threads[threadIndex].join();
	}
}

void rasterizeTriangleInBoundingBox(RenderTarget& target, const RenderSettings& settings, const Triangle& t, const cml::ivec2 bbMin, cml::ivec2 bbMax) {
	const cml::ivec2 v0 = coordinateNDCtoRaster(t.v1.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight());
	const cml::ivec2 v1 = coordinateNDCtoRaster(t.v2.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight());
	const cml::ivec2 v2 = coordinateNDCtoRaster(t.v3.position, (uint32_t)target.getWidth(), (uint32_t)target.getHeight());

	const cml::ivec2 edgeVectors[3] = {
		v0 - v2,
		v1 - v0,
		v2 - v1
	};

	int minX = std::min(bbMin.x, std::min(std::min(v0.x, v1.x), v2.x));
	int maxX = std::max(bbMax.x, std::max(std::max(v0.x, v1.x), v2.x));
	int xExtent = maxX - minX;

	float area = (float)edgeFunction(v0, edgeVectors[0], v1);
	//skip triangles without surface + backface culling
	if (area >= 0.f) {
		return;
	}

	__m128i edge0Y = _mm_set1_epi32(edgeVectors[0].y);
	__m128i edge1Y = _mm_set1_epi32(edgeVectors[1].y);
	__m128i edge2Y = _mm_set1_epi32(edgeVectors[2].y);

	//all edges in one register, used for search
	__m128i edgesX = _mm_setr_epi32(edgeVectors[2].x, edgeVectors[0].x, edgeVectors[1].x, 0);
	__m128i edgesY = _mm_setr_epi32(edgeVectors[2].y, edgeVectors[0].y, edgeVectors[1].y, 0);

	cml::ivec2 startPoint;
	if (v0.y >= v1.y && v0.y >= v2.y) startPoint = v0;
	else if (v1.y >= v2.y) startPoint = v1;
	else startPoint = v2;

	startPoint.x = std::max(bbMin.x, std::min(startPoint.y, bbMax.x));
	startPoint.y = std::min(startPoint.y, bbMax.y);

	union { __m128i bStart; int bStart_arr[4]; };
	bStart_arr[1] = edgeFunction(v0, edgeVectors[0], startPoint);
	bStart_arr[2] = edgeFunction(v1, edgeVectors[1], startPoint);
	bStart_arr[0] = edgeFunction(v2, edgeVectors[2], startPoint);

	//writes four pixels using SIMD register inputs
	//returns true if all pixels were in triangle
	//no captures as they are slower by a few ms
	auto writeFragments = [](__m128i& bX, __m128i& bY, __m128i& bZ, int y, int xCo_arr[4], float area, const RenderSettings& settings, RenderTarget& target, 
		const Triangle& t, const cml::ivec2 bbMin, const cml::ivec2 bbMax, __m128i xCo) {
		union { __m128 bNormalizedX; float bNormalizedXArr[4]; };
		union { __m128 bNormalizedY; float bNormalizedYArr[4]; };
		union { __m128 bNormalizedZ; float bNormalizedZArr[4]; };

		__m128 areaReg = _mm_set1_ps(area);
		bNormalizedX = _mm_div_ps(_mm_cvtepi32_ps(bX), areaReg);
		bNormalizedY = _mm_div_ps(_mm_cvtepi32_ps(bY), areaReg);
		bNormalizedZ = _mm_div_ps(_mm_cvtepi32_ps(bZ), areaReg);

		InterpolationResult v = interpolateVertexDataSIMD(t, bNormalizedX, bNormalizedY, bNormalizedZ);
		ColorSIMD colors = settings.shadingFunction(v, settings.shaderInput);
		union { __m128i valid; int valid_arr[4]; };
		valid = isBarycentricValidMultipleSIMD(bX, bY, bZ);

		__m128i xMin = _mm_set1_epi32(bbMin.x - 1); //SSE only provides greather than, not greater equal
		__m128i xMax = _mm_set1_epi32(bbMax.x + 1);

		valid = _mm_and_si128(valid, _mm_cmpgt_epi32(xCo, xMin));
		valid = _mm_and_si128(valid, _mm_cmplt_epi32(xCo, xMax));

		union { __m128i depth; int depth_arr[4]; };
		depth = _mm_cvtps_epi32(_mm_mul_ps(v.posZ, _mm_set1_ps((float)-INT_MAX)));

		target.writeDepthTestSIMD(xCo_arr, (size_t)y, colors, valid, depth_arr);

		//valid = isBarycentricValidMultipleSIMD(bX, bY, bZ); //fixes problem, but why?
		return valid_arr[0] && valid_arr[1] && valid_arr[2];
	};

	int minY = std::max(bbMin.y, std::min(std::min(v0.y, v1.y), v2.y));
	for (int y = startPoint.y; y >= minY; y--) {
		//if we landed on a invalid point we left the triangle -> search for new start point
		//in this case only one pixel is processed at a time, with the barycentric vector being SIMDed
		//this is because most of the time only the first pixel to the left and right is tested
		if (!(isBarycentricVaildSingleSIMD(bStart))) {
			__m128i bRight = bStart;
			__m128i bLeft = bStart;
			//some thin triangles don't have a valid pixel, because of undersampling, only search in radius of extent
			for (int xOffset = 1; xOffset < xExtent; xOffset++) {
				//check right
				bRight = _mm_add_epi32(bRight, edgesY);
				if (isBarycentricVaildSingleSIMD(bRight) && startPoint.x + xOffset <= bbMax.x) {
					startPoint.x += xOffset;
					bStart = bRight;
					break;
				}
				//check left
				bLeft = _mm_sub_epi32(bLeft, edgesY);
				if (isBarycentricVaildSingleSIMD(bLeft) && startPoint.x - xOffset >= bbMin.x) {
					startPoint.x -= xOffset;
					bStart = bLeft;
					break;
				}
			}
		}

		//set point to start point
		cml::ivec2 p = startPoint;

		union { __m128i bX; int bXArr[4]; };
		union { __m128i bY; int bYArr[4]; };
		union { __m128i bZ; int bZArr[4]; };

		bX = _mm_set1_epi32(bStart_arr[0]);
		bY = _mm_set1_epi32(bStart_arr[1]);
		bZ = _mm_set1_epi32(bStart_arr[2]);

		__m128i lane = _mm_setr_epi32(0, 1, 2, 3);

		bY = _mm_add_epi32(bY, _mm_mullo_epi32(edge0Y, lane));
		bZ = _mm_add_epi32(bZ, _mm_mullo_epi32(edge1Y, lane));
		bX = _mm_add_epi32(bX, _mm_mullo_epi32(edge2Y, lane));

		__m128i four = _mm_set1_epi32(4);
		__m128i edge0Y_x4 = _mm_mullo_epi32(edge0Y, four);
		__m128i edge1Y_x4 = _mm_mullo_epi32(edge1Y, four);
		__m128i edge2Y_x4 = _mm_mullo_epi32(edge2Y, four);

		//go right
		bool inTriangle = true;
		while (inTriangle) {

			union { __m128i xCo; int xCo_arr[4]; };
			xCo = _mm_add_epi32(_mm_set1_epi32(p.x), lane);

			inTriangle = writeFragments(bX, bY, bZ, p.y, xCo_arr, area, settings, target, t, bbMin, bbMax, xCo);

			bY = _mm_add_epi32(bY, edge0Y_x4);
			bZ = _mm_add_epi32(bZ, edge1Y_x4);
			bX = _mm_add_epi32(bX, edge2Y_x4);

			p.x += 4;
		}
		//set point to start point
		p = startPoint;
		//already checked start point
		p.x--;

		bY = _mm_set1_epi32(bStart_arr[1] - edgeVectors[0].y);
		bZ = _mm_set1_epi32(bStart_arr[2] - edgeVectors[1].y);
		bX = _mm_set1_epi32(bStart_arr[0] - edgeVectors[2].y);

		bY = _mm_sub_epi32(bY, _mm_mullo_epi32(edge0Y, lane));
		bZ = _mm_sub_epi32(bZ, _mm_mullo_epi32(edge1Y, lane));
		bX = _mm_sub_epi32(bX, _mm_mullo_epi32(edge2Y, lane));

		//go left
		inTriangle = true;
		while (inTriangle) {

			union { __m128i xCo; int xCo_arr[4]; };
			xCo = _mm_sub_epi32(_mm_set1_epi32(p.x), lane);

			inTriangle = writeFragments(bX, bY, bZ, p.y, xCo_arr, area, settings, target, t, bbMin, bbMax, xCo);

			bY = _mm_sub_epi32(bY, edge0Y_x4);
			bZ = _mm_sub_epi32(bZ, edge1Y_x4);
			bX = _mm_sub_epi32(bX, edge2Y_x4);

			p.x -= 4;
		}

		//decrement start point y
		startPoint.y--;
		//adjust start point barycentric
		bStart = _mm_add_epi32(bStart, edgesX);
	}
}

int edgeFunction(const cml::ivec2& v, const cml::ivec2& deltaV, const cml::ivec2& p) {
	return (p.x - v.x) * deltaV.y - (p.y - v.y) * deltaV.x;
};

bool isBarycentricValid(const cml::ivec3 b) {
	return (b.x <= 0) && (b.y <= 0) && (b.z <= 0);
}

bool isBarycentricVaildSingleSIMD(const __m128i& b) {
	union { __m128i result; int result_arr[4]; };
	result = _mm_cmplt_epi32(b, _mm_set1_epi32(1)); //less than 1 = less/equal than 0
	return result_arr[0] && result_arr[1] && result_arr[2];
}

__m128i isBarycentricValidMultipleSIMD(const __m128i& x, const __m128i& y, const __m128i& z) {
	__m128i cprX = _mm_cmplt_epi32(x, _mm_set1_epi32(1));
	__m128i cprY = _mm_cmplt_epi32(y, _mm_set1_epi32(1));
	__m128i cprZ = _mm_cmplt_epi32(z, _mm_set1_epi32(1));

	return _mm_and_si128(_mm_and_si128(cprX, cprY), cprZ);
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

InterpolationResult interpolateVertexDataSIMD(const Triangle& t, const __m128& b0, const __m128& b1, const __m128& b2) {
	InterpolationResult result;

	//position
	__m128 p0X = _mm_set1_ps(t.v1.position.x);
	__m128 p0Y = _mm_set1_ps(t.v1.position.y);
	__m128 p0Z = _mm_set1_ps(t.v1.position.z);

	__m128 p1X = _mm_set1_ps(t.v2.position.x);
	__m128 p1Y = _mm_set1_ps(t.v2.position.y);
	__m128 p1Z = _mm_set1_ps(t.v2.position.z);

	__m128 p2X = _mm_set1_ps(t.v3.position.x);
	__m128 p2Y = _mm_set1_ps(t.v3.position.y);
	__m128 p2Z = _mm_set1_ps(t.v3.position.z);

	result.posX = _mm_mul_ps(p0X, b0);
	result.posX = _mm_add_ps(result.posX, _mm_mul_ps(p1X, b1));
	result.posX = _mm_add_ps(result.posX, _mm_mul_ps(p2X, b2));

	result.posY = _mm_mul_ps(p0Y, b0);
	result.posY = _mm_add_ps(result.posY, _mm_mul_ps(p1Y, b1));
	result.posY = _mm_add_ps(result.posY, _mm_mul_ps(p2Y, b2));

	result.posZ = _mm_mul_ps(p0Z, b0);
	result.posZ = _mm_add_ps(result.posZ, _mm_mul_ps(p1Z, b1));
	result.posZ = _mm_add_ps(result.posZ, _mm_mul_ps(p2Z, b2));

	//normal
	__m128 n0X = _mm_set1_ps(t.v1.normal.x);
	__m128 n0Y = _mm_set1_ps(t.v1.normal.y);
	__m128 n0Z = _mm_set1_ps(t.v1.normal.z);
		   
	__m128 n1X = _mm_set1_ps(t.v2.normal.x);
	__m128 n1Y = _mm_set1_ps(t.v2.normal.y);
	__m128 n1Z = _mm_set1_ps(t.v2.normal.z);
		   
	__m128 n2X = _mm_set1_ps(t.v3.normal.x);
	__m128 n2Y = _mm_set1_ps(t.v3.normal.y);
	__m128 n2Z = _mm_set1_ps(t.v3.normal.z);

	result.normalX = _mm_mul_ps(n0X, b0);
	result.normalX = _mm_add_ps(result.normalX, _mm_mul_ps(n1X, b1));
	result.normalX = _mm_add_ps(result.normalX, _mm_mul_ps(n2X, b2));

	result.normalY = _mm_mul_ps(n0Y, b0);
	result.normalY = _mm_add_ps(result.normalY, _mm_mul_ps(n1Y, b1));
	result.normalY = _mm_add_ps(result.normalY, _mm_mul_ps(n2Y, b2));

	result.normalZ = _mm_mul_ps(n0Z, b0);
	result.normalZ = _mm_add_ps(result.normalZ, _mm_mul_ps(n1Z, b1));
	result.normalZ = _mm_add_ps(result.normalZ, _mm_mul_ps(n2Z, b2));

	//UV
	__m128 uv0X = _mm_set1_ps(t.v1.uv.x);
	__m128 uv0Y = _mm_set1_ps(t.v1.uv.y);

	__m128 uv1X = _mm_set1_ps(t.v2.uv.x);
	__m128 uv1Y = _mm_set1_ps(t.v2.uv.y);

	__m128 uv2X = _mm_set1_ps(t.v3.uv.x);
	__m128 uv2Y = _mm_set1_ps(t.v3.uv.y);

	result.uvX = _mm_mul_ps(uv0X, b0);
	result.uvX = _mm_add_ps(result.uvX, _mm_mul_ps(uv1X, b1));
	result.uvX = _mm_add_ps(result.uvX, _mm_mul_ps(uv2X, b2));

	result.uvY = _mm_mul_ps(uv0Y, b0);
	result.uvY = _mm_add_ps(result.uvY, _mm_mul_ps(uv1Y, b1));
	result.uvY = _mm_add_ps(result.uvY, _mm_mul_ps(uv2Y, b2));

	return result;
}

Vertex interpolateVertexData(const Triangle& t, const cml::vec3 b) {
	//res.position = t.v1.position * b.x + t.v2.position * b.y + t.v3.position * b.z;
	//res.normal = t.v1.normal * b.x + t.v2.normal * b.y + t.v3.normal * b.z;
	//res.uv = t.v1.uv * b.x + t.v2.uv * b.y + t.v3.uv * b.z;
	//FIXME indexed access to triangle to replace duplicate code
	const auto& p0 = t.v1.position;
	const auto& n0 = t.v1.normal;
	__m128 posReg0  = _mm_setr_ps(p0.x, p0.y, p0.z, t.v1.uv.x);
	__m128 normReg0 = _mm_setr_ps(n0.x, n0.y, n0.z, t.v1.uv.y);

	const auto& p1 = t.v2.position;
	const auto& n1 = t.v2.normal;
	__m128 posReg1  = _mm_setr_ps(p1.x, p1.y, p1.z, t.v2.uv.x);
	__m128 normReg1 = _mm_setr_ps(n1.x, n1.y, n1.z, t.v2.uv.y);

	const auto& p2 = t.v3.position;
	const auto& n2 = t.v3.normal;
	__m128 posReg2  = _mm_setr_ps(p2.x, p2.y, p2.z, t.v3.uv.x);
	__m128 normReg2 = _mm_setr_ps(n2.x, n2.y, n2.z, t.v3.uv.y);

	__m128 b0Reg = _mm_set1_ps(b.x);
	__m128 b1Reg = _mm_set1_ps(b.y);
	__m128 b2Reg = _mm_set1_ps(b.z);

	posReg0 = _mm_mul_ps(posReg0, b0Reg);
	posReg1 = _mm_mul_ps(posReg1, b1Reg);
	posReg2 = _mm_mul_ps(posReg2, b2Reg);

	normReg0 = _mm_mul_ps(normReg0, b0Reg);
	normReg1 = _mm_mul_ps(normReg1, b1Reg);
	normReg2 = _mm_mul_ps(normReg2, b2Reg);

	union { __m128 posResReg;  float posResArr[4]; };
	union { __m128 normResReg; float normResArr[4]; };

	posResReg  = _mm_add_ps(posReg0,  _mm_add_ps(posReg1,  posReg2));
	normResReg = _mm_add_ps(normReg0, _mm_add_ps(normReg1, normReg2));

	Vertex v;
	v.position = cml::vec3(posResArr[0],  posResArr[1],  posResArr[2]);
	v.normal   = cml::vec3(normResArr[0], normResArr[1], normResArr[2]);
	v.uv = cml::vec2(posResArr[3], normResArr[3]);

	return v;
}