#pragma once
#include "pch.h"

namespace cml {

	//forward declare for casting operator
	struct ivec2;

	struct vec2 {
		vec2();
		vec2(float xIn, float yIn);
		vec2(float value);
		float x;
		float y;
		float& operator [](int index);
		vec2 operator *(const float& scalar) const;
		vec2 operator /(const float& scalar) const;
		vec2 operator +(const float& scalar) const;
		vec2 operator -(const float& scalar) const;
		vec2 operator +(const vec2& other) const;
		vec2 operator -(const vec2& other) const;

		explicit operator ivec2() const;
	};

	struct vec3 {
		vec3();
		vec3(float xIn, float yIn, float zIn);
		vec3(float value);
		float x;
		float y;
		float z;
		float& operator [](int index);
		vec3 operator *(const float& scalar) const;
		vec3 operator /(const float& scalar) const;
		vec3 operator +(const float& scalar) const;
		vec3 operator -(const float& scalar) const;
		vec3 operator +(const vec3& other) const;
		vec3 operator -(const vec3& other) const;
	};

	struct vec4 {
		vec4();
		vec4(float xIn, float yIn, float zIn, float wIn);
		vec4(float value);
		vec4(cml::vec3 vIn, float wIn);
		float x;
		float y;
		float z;
		float w;
		float& operator [](int index);
		vec4 operator *(const float& scalar) const;
		vec4 operator /(const float& scalar) const;
		vec4 operator +(const float& scalar) const;
		vec4 operator -(const float& scalar) const;
		vec4 operator +(const vec4& other) const;
		vec4 operator -(const vec4& other) const;
	};

	struct ivec2 {
		ivec2();
		ivec2(int xIn, int yIn);
		ivec2(int value);
		int x;
		int y;
		int& operator [](const int& index);
		ivec2 operator *(const int& scalar) const;
		ivec2 operator /(const int& scalar) const;
		ivec2 operator +(const int& scalar) const;
		ivec2 operator -(const int& scalar) const;
		ivec2 operator +(const ivec2& other) const;
		ivec2 operator -(const ivec2& other) const;

		explicit operator vec2() const;
	};

	vec3 normalize(const vec3& v);
	float length(const vec3& v);
	float dot(const vec3& v1, const vec3& v2);
	vec3 cross(const vec3& a, const vec3& b);
}
