#pragma once
#include "pch.h"
#include "CML/Vector.h"
#include "Mesh.h"
#include "ShaderInput.h"

namespace ShadingFunctions {
	cml::vec3 normalVis(const Vertex& v, const ShaderInput& input);
	cml::vec3 positionVis(const Vertex& v, const ShaderInput& input);
	cml::vec3 uvVis(const Vertex& v, const ShaderInput& input);
	cml::vec3 depthVis(const Vertex& v, const ShaderInput& input);
	cml::vec3 frontLight(const Vertex& v, const ShaderInput& input);
	cml::vec3 texture(const Vertex& v, const ShaderInput& input);
	cml::vec3 texturedLit(const Vertex& v, const ShaderInput& input);
}
