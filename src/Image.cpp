#include "pch.h"
#include "Image.h"
#include "CML/Common.h"

// ------------ RGBImage -------------

RGBImageRow::RGBImageRow(size_t length) {
	m_data = new cml::vec3[length];
}

RGBImageRow::~RGBImageRow() {
	delete m_data;
}

cml::vec3& RGBImageRow::operator [](size_t index) {
	return m_data[index];
}

RGBImage::RGBImage(size_t width, size_t height) : m_width(width), m_height(height){
	void* memory = operator new[](width* height * sizeof(cml::vec3));
	m_data = static_cast<RGBImageRow*>(memory);
	for (size_t i = 0; i < height; i++) {
		new(&m_data[i]) RGBImageRow(width);
	}
}

void RGBImage::clear(const cml::vec3& color) {
	for (size_t x = 0; x < m_width; x++) {
		for (size_t y = 0; y < m_height; y++) {
			(*this)[y][x] = color;
		}
	}
}

cml::vec3 RGBImage::sample(const cml::vec2& uv) {
	int widthInt = (int)m_width;
	int heightInt = (int)m_height;

	cml::vec2 raster = cml::vec2(uv.x * (widthInt - 1), uv.y * (heightInt - 1));

	cml::ivec2 uvCeil = cml::ivec2((int)std::ceil(raster.x), (int)std::ceil(raster.y));
	uvCeil = cml::ivec2(uvCeil.x % widthInt, uvCeil.y % heightInt);

	cml::ivec2 uvFloor = cml::ivec2((int)std::floor(raster.x), (int)std::floor(raster.y));
	cml::vec2 t = raster - (cml::vec2)uvFloor;

	cml::vec3 ul = (*this)[(size_t)uvCeil.y][(size_t)uvFloor.x];
	cml::vec3 ur = (*this)[(size_t)uvCeil.y][(size_t)uvCeil.x];
	cml::vec3 u = cml::lerp(ul, ur, t.x);

	cml::vec3 ll = (*this)[(size_t)uvFloor.y][(size_t)uvFloor.x];
	cml::vec3 lr = (*this)[(size_t)uvFloor.y][(size_t)uvCeil.x];
	cml::vec3 l = cml::lerp(ll, lr, t.x);

	return cml::lerp(l, u, t.y);
}

size_t RGBImage::getWidth() {
	return m_width;
}

size_t RGBImage::getHeight() {
	return m_height;
}

RGBImageRow& RGBImage::operator [](size_t index) {
	return m_data[index];
}

RGBImage::~RGBImage() {
	delete m_data;
}

// --------------- IntImage -----------

IntImageRow::IntImageRow(size_t length) {
	m_data = new int[length];
}

IntImage::~IntImage() {
	delete m_data;
}

int& IntImageRow::operator [](size_t index) {
	return m_data[index];
}

IntImage::IntImage(size_t width, size_t height) : m_width(width), m_height(height) {
	void* memory = operator new[](width* height * sizeof(int));
	m_data = static_cast<IntImageRow*>(memory);
	for (size_t i = 0; i < height; i++) {
		new(&m_data[i]) IntImageRow(width);
	}
}

void IntImage::clear(const int& value) {
	for (size_t x = 0; x < m_width; x++) {
		for (size_t y = 0; y < m_height; y++) {
			(*this)[y][x] = value;
		}
	}
}

size_t IntImage::getWidth() {
	return m_width;
}

size_t IntImage::getHeight() {
	return m_height;
}

IntImageRow& IntImage::operator [](size_t index) {
	return m_data[index];
}

IntImageRow::~IntImageRow() {
	delete m_data;
}