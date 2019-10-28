#include "pch.h"
#include "Image.h"
#include "CML/Common.h"

// ------------ RGBImage -------------

RGBImageRow::RGBImageRow(int length) {
	m_data = new cml::vec3[length];
}

RGBImageRow::~RGBImageRow() {
	delete m_data;
}

cml::vec3& RGBImageRow::operator [](int index) {
	return m_data[index];
}

RGBImage::RGBImage(int width, int height) : m_width(width), m_height(height){
	void* memory = operator new[](width* height * sizeof(cml::vec3));
	m_data = static_cast<RGBImageRow*>(memory);
	for (int i = 0; i < height; i++) {
		new(&m_data[i]) RGBImageRow(width);
	}
}

void RGBImage::clear(const cml::vec3& color) {
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++) {
			(*this)[y][x] = color;
		}
	}
}

cml::vec3 RGBImage::sample(const cml::vec2& uv) {
	cml::vec2 raster = cml::vec2(uv.x * (m_width - 1), uv.y * (m_height - 1));

	cml::vec2 uvCeil = cml::vec2(std::ceil(raster.x), std::ceil(raster.y));
	uvCeil = cml::vec2((int)uvCeil.x % m_width, (int)uvCeil.y % m_height);

	cml::vec2 uvFloor = cml::vec2(std::floor(raster.x), std::floor(raster.y));
	cml::vec2 t = raster - uvFloor;

	cml::vec3 ul = (*this)[uvCeil.y][uvFloor.x];
	cml::vec3 ur = (*this)[uvCeil.y][uvCeil.x];
	cml::vec3 u = cml::lerp(ul, ur, t.x);

	cml::vec3 ll = (*this)[uvFloor.y][uvFloor.x];
	cml::vec3 lr = (*this)[uvFloor.y][uvCeil.x];
	cml::vec3 l = cml::lerp(ll, lr, t.x);

	return cml::lerp(l, u, t.y);
}

int RGBImage::getWidth() {
	return m_width;
}

int RGBImage::getHeight() {
	return m_height;
}

RGBImageRow& RGBImage::operator [](int index) {
	return m_data[index];
}

RGBImage::~RGBImage() {
	delete m_data;
}

// --------------- IntImage -----------

IntImageRow::IntImageRow(int length) {
	m_data = new int[length];
}

IntImage::~IntImage() {
	delete m_data;
}

int& IntImageRow::operator [](int index) {
	return m_data[index];
}

IntImage::IntImage(int width, int height) : m_width(width), m_height(height) {
	void* memory = operator new[](width* height * sizeof(int));
	m_data = static_cast<IntImageRow*>(memory);
	for (int i = 0; i < height; i++) {
		new(&m_data[i]) IntImageRow(width);
	}
}

void IntImage::clear(const int& value) {
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++) {
			(*this)[y][x] = value;
		}
	}
}

int IntImage::getWidth() {
	return m_width;
}

int IntImage::getHeight() {
	return m_height;
}

IntImageRow& IntImage::operator [](int index) {
	return m_data[index];
}

IntImageRow::~IntImageRow() {
	delete m_data;
}