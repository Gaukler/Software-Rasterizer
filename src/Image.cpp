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
	//TODO clamp/wrap uv

	int widthInt = (int)m_width;
	int heightInt = (int)m_height;

	cml::vec2 raster = cml::vec2(uv.x * (widthInt - 1), uv.y * (heightInt - 1));

	cml::ivec2 uvFloor = cml::ivec2(raster);
	cml::ivec2 uvCeil = uvFloor + cml::ivec2(1, 1);
	cml::vec2 t = raster - (cml::vec2)uvFloor;

	cml::vec3 ul = (*this)[(size_t)uvCeil.y][(size_t)uvFloor.x];
	cml::vec3 ur = (*this)[(size_t)uvCeil.y][(size_t)uvCeil.x];

	cml::vec3 ll = (*this)[(size_t)uvFloor.y][(size_t)uvFloor.x];
	cml::vec3 lr = (*this)[(size_t)uvFloor.y][(size_t)uvCeil.x];

	//manual lerp to allow for register reusage
	const float tInvX = 1.f - t.x;

	const __m128 txReg    = _mm_set1_ps(t.x);;
	const __m128 txInvReg = _mm_set_ps1(tInvX);

	//upper values
	__m128 ulReg = _mm_setr_ps(ul.x, ul.y, ul.z, 0.f);
	__m128 urReg = _mm_setr_ps(ur.x, ur.y, ur.z, 0.f);	
	
	ulReg = _mm_mul_ps(ulReg, txInvReg);
	urReg = _mm_mul_ps(urReg, txReg);
	__m128 upper = _mm_add_ps(ulReg, urReg);

	//lower values
	__m128 llReg = _mm_setr_ps(ll.x, ll.y, ll.z, 0.f);
	__m128 lrReg = _mm_setr_ps(lr.x, lr.y, lr.z, 0.f);

	llReg = _mm_mul_ps(llReg, txInvReg);
	lrReg = _mm_mul_ps(lrReg, txReg);
	__m128 lower = _mm_add_ps(llReg, lrReg);

	//interpolate y-axis
	const float tInvY = 1.f - t.y;

	const __m128 tyReg    = _mm_set1_ps(t.y);
	const __m128 tyInvReg = _mm_set_ps1(tInvY);

	upper = _mm_mul_ps(upper, tyReg);
	lower = _mm_mul_ps(lower, tyInvReg);

	union { __m128 pixelReg; float pixelArr[4]; };
	pixelReg = _mm_add_ps(lower, upper);
	return cml::vec3(pixelArr[0], pixelArr[1], pixelArr[2]);
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