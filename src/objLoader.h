#pragma once
#include "pch.h"
#include "Mesh.h"

namespace objLoader {
	Mesh* loadOBJ(const std::string& path);
}