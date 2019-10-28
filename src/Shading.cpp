#include "pch.h"
#include "Shading.h"

namespace ShadingFunctions {
	cml::vec3 barycentricVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input) {
		return b;
	}

	cml::vec3 normalVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input) {
		return cml::normalize(t.v1.normal * b.x + t.v2.normal * b.y + t.v3.normal * b.z);
	}

	cml::vec3 positionVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input) {
		return t.v1.position * b.x + t.v2.position * b.y + t.v3.position * b.z;
	}

	cml::vec3 uvVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input) {
		cml::vec2 uv = t.v1.uv * b.x + t.v2.uv * b.y + t.v3.uv * b.z;
		return cml::vec3(uv.x, uv.y, 0.f);
	}

	cml::vec3 depthVis(const Triangle& t, const cml::vec3& b, const ShaderInput& input) {
		return cml::vec3((t.v1.position * b.x + t.v2.position * b.y + t.v3.position * b.z).z);
	}

	cml::vec3 frontLight(const Triangle& t, const cml::vec3& b, const ShaderInput& input) {
		return cml::dot(cml::normalize(t.v1.position * b.x + t.v2.position * b.y + t.v3.position * b.z), cml::normalize(cml::vec3(1.f, 1.f, 1.f)));
	}

	cml::vec3 texture(const Triangle& t, const cml::vec3& b, const ShaderInput& input) {
		cml::vec2 uv = t.v1.uv * b.x + t.v2.uv * b.y + t.v3.uv * b.z;
		return input.texture->sample(uv);
	}

	cml::vec3 texturedLit(const Triangle& t, const cml::vec3& b, const ShaderInput& input) {
		cml::vec2 uv = t.v1.uv * b.x + t.v2.uv * b.y + t.v3.uv * b.z;
		cml::vec3 normal = cml::normalize(t.v1.normal * b.x + t.v2.normal * b.y + t.v3.normal * b.z);
		cml::vec3 light = cml::normalize(input.lightPosition);
		return input.texture->sample(uv) * (std::max(cml::dot(light, normal), 0.f) + 0.2f);
	}
}