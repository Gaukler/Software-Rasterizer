#pragma once
#include "pch.h"
#include "CML/Vector.h"

struct Vertex {
	cml::vec3 position;
	cml::vec3 normal;
	cml::vec2 uv;
};

struct Triangle {
	Vertex v1;
	Vertex v2;
	Vertex v3;
};

struct Mesh {
	std::vector<Triangle> triangles;
};