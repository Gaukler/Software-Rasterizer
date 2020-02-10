#include "pch.h"
#include "Shading.h"

namespace ShadingFunctions {

	//disable warning for unused parameter "input"
	#ifdef _MSC_VER
	#pragma warning(disable: 4100)
	#endif

	cml::vec3 normalVis(const Vertex& v, const ShaderInput& input) {
		return cml::normalize(v.normal);
	}

	cml::vec3 positionVis(const Vertex& v, const ShaderInput& input) {
		return v.position;
	}

	cml::vec3 uvVis(const Vertex& v, const ShaderInput& input) {
		return cml::vec3(v.uv.x, v.uv.y, 0.f);
	}

	cml::vec3 depthVis(const Vertex& v, const ShaderInput& input) {
		return cml::vec3(v.position.z);
	}

	cml::vec3 frontLight(const Vertex& v, const ShaderInput& input) {
		return cml::dot(cml::normalize(v.normal), cml::normalize(cml::vec3(1.f, 1.f, 1.f)));
	}

	cml::vec3 texture(const Vertex& v, const ShaderInput& input) {
		return input.texture->sample(v.uv);
	}

	cml::vec3 texturedLit(const Vertex& v, const ShaderInput& input) {
		cml::vec3 light = cml::normalize(input.lightPosition);
		return input.texture->sample(v.uv) * (std::max(cml::dot(light, cml::normalize(v.normal)), 0.f) + 0.2f);
	}

	//reenable warning
	#ifdef _MSC_VER
	#pragma warning(default: 4100)
	#endif
}