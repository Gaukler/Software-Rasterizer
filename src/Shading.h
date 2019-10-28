#pragma once
#include "pch.h"
#include "CML/Vector.h"
#include "Mesh.h"
#include "ShaderInput.h"

namespace ShadingFunctions {
	cml::vec3 barycentricVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
	cml::vec3 normalVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
	cml::vec3 positionVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
	cml::vec3 uvVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
	cml::vec3 depthVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
	cml::vec3 frontLight(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
	cml::vec3 texture(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
	cml::vec3 texturedLit(const Triangle& t, const cml::vec3& b, const ShaderInput& input);
}
