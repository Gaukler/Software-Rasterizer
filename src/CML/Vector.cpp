#include "../pch.h"
#include "Vector.h"

namespace cml {

	float length(const vec3& v) {
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	vec3 normalize(const vec3& v) {
		return v / length(v);
	}

	float dot(const vec3& v1, const vec3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	vec3 cross(const vec3& a, const vec3& b) {
		vec3 res;
		res.x = a.y * b.z - a.z * b.y;
		res.y = a.z * b.x - a.x * b.z;
		res.z = a.x * b.y - a.y * b.x;
		return res;
	}

	// ---------- vec3 -----------------
	vec3::vec3() : x(0.f), y(0.f), z(0.f) {

	}

	vec3::vec3(float xIn, float yIn, float zIn) : x(xIn), y(yIn), z(zIn) {

	}

	vec3::vec3(float value) : x(value), y(value), z(value) {

	}

	float& vec3::operator [](int index) {
		return (&x)[index];
	}

	vec3 vec3::operator *(const float& scalar) const {
		vec3 result;
		result.x = x * scalar;
		result.y = y * scalar;
		result.z = z * scalar;
		return result;
	}

	vec3 vec3::operator /(const float& scalar) const {
		vec3 result;
		result.x = x / scalar;
		result.y = y / scalar;
		result.z = z / scalar;
		return result;
	}

	vec3 vec3::operator +(const float& scalar) const {
		vec3 result;
		result.x = x + scalar;
		result.y = y + scalar;
		result.z = z + scalar;
		return result;
	}

	vec3 vec3::operator -(const float& scalar) const {
		vec3 result;
		result.x = x - scalar;
		result.y = y - scalar;
		result.z = z - scalar;
		return result;
	}

	vec3 vec3::operator +(const vec3& other) const {
		vec3 result;
		result.x = x + other.x;
		result.y = y + other.y;
		result.z = z + other.z;
		return result;
	}

	vec3 vec3::operator -(const vec3& other) const {
		vec3 result;
		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;
		return result;
	}

	// ---------- vec4 -----------------
	vec4::vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {

	}

	vec4::vec4(float xIn, float yIn, float zIn, float wIn) : x(xIn), y(yIn), z(zIn), w(wIn) {

	}

	vec4::vec4(float value) : x(value), y(value), z(value), w(value) {

	}

	vec4::vec4(cml::vec3 vIn, float wIn) : x(vIn.x), y(vIn.y), z(vIn.z), w(wIn) {

	}

	float& vec4::operator [](int index) {
		return (&x)[index];
	}

	vec4 vec4::operator *(const float& scalar) const {
		vec4 result;
		result.x = x * scalar;
		result.y = y * scalar;
		result.z = z * scalar;
		result.w = w * scalar;
		return result;
	}

	vec4 vec4::operator /(const float& scalar) const {
		vec4 result;
		result.x = x / scalar;
		result.y = y / scalar;
		result.z = z / scalar;
		result.w = w / scalar;
		return result;
	}

	vec4 vec4::operator +(const float& scalar) const {
		vec4 result;
		result.x = x + scalar;
		result.y = y + scalar;
		result.z = z + scalar;
		result.w = w + scalar;
		return result;
	}

	vec4 vec4::operator -(const float& scalar) const {
		vec4 result;
		result.x = x - scalar;
		result.y = y - scalar;
		result.z = z - scalar;
		result.w = w - scalar;
		return result;
	}

	vec4 vec4::operator +(const vec4& other) const {
		vec4 result;
		result.x = x + other.x;
		result.y = y + other.y;
		result.z = z + other.z;
		result.w = w + other.w;
		return result;
	}

	vec4 vec4::operator -(const vec4& other) const {
		vec4 result;
		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;
		result.w = w - other.w;
		return result;
	}

	// ---------- vec2 -----------------

	vec2::vec2() : x(0.f), y(0.f) {

	}

	vec2::vec2(float xIn, float yIn) : x(xIn), y(yIn) {

	}

	vec2::vec2(float value) : x(value), y(value) {

	}

	float& vec2::operator [](int index) {
		return (&x)[index];
	}

	vec2 vec2::operator *(const float& scalar) const {
		vec2 result;
		result.x = x * scalar;
		result.y = y * scalar;
		return result;
	}

	vec2 vec2::operator /(const float& scalar) const {
		vec2 result;
		result.x = x / scalar;
		result.y = y / scalar;
		return result;
	}

	vec2 vec2::operator +(const float& scalar) const {
		vec2 result;
		result.x = x + scalar;
		result.y = y + scalar;
		return result;
	}

	vec2 vec2::operator -(const float& scalar) const {
		vec2 result;
		result.x = x - scalar;
		result.y = y - scalar;
		return result;
		return *this;
	}

	vec2 vec2::operator +(const vec2& other) const {
		vec2 result;
		result.x = x + other.x;
		result.y = y + other.y;
		return result;
	}

	vec2 vec2::operator -(const vec2& other) const {
		vec2 result;
		result.x = x - other.x;
		result.y = y - other.y;
		return result;
	}

	// ---------- ivec2 -----------------

	ivec2::ivec2() : x(0), y(0) {

	}

	ivec2::ivec2(int xIn, int yIn) : x(xIn), y(yIn) {

	}

	ivec2::ivec2(int value) : x(value), y(value) {

	}

	int& ivec2::operator [](const int& index){
		return (&x)[index];
	}

	ivec2 ivec2::operator *(const int& scalar) const {
		ivec2 result;
		result.x = x * scalar;
		result.y = y * scalar;
		return result;
	}

	ivec2 ivec2::operator /(const int& scalar) const {
		ivec2 result;
		result.x = x / scalar;
		result.y = y / scalar;
		return result;
	}

	ivec2 ivec2::operator +(const int& scalar) const {
		ivec2 result;
		result.x = x + scalar;
		result.y = y + scalar;
		return result;
	}

	ivec2 ivec2::operator -(const int& scalar) const {
		ivec2 result;
		result.x = x - scalar;
		result.y = y - scalar;
		return result;
		return *this;
	}

	ivec2 ivec2::operator +(const ivec2& other) const {
		ivec2 result;
		result.x = x + other.x;
		result.y = y + other.y;
		return result;
	}

	ivec2 ivec2::operator -(const ivec2& other) const {
		ivec2 result;
		result.x = x - other.x;
		result.y = y - other.y;
		return result;
	}
}
