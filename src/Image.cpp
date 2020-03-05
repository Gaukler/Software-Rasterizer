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

ColorSIMD RGBImage::sampleSIMD(__m128 uvX, __m128 uvY) {
	//clamp uvs
	//not 0/1 because floor/ceil must not be max value
	__m128 min = _mm_set1_ps(0.001f); 
	__m128 max = _mm_set1_ps(0.999f); 

	uvX = _mm_min_ps(_mm_max_ps(uvX, min), max);
	uvY = _mm_min_ps(_mm_max_ps(uvY, min), max);


	//floor
	__m128i width  = _mm_set1_epi32((int)m_width  - 1);
	__m128i height = _mm_set1_epi32((int)m_height - 1);
	
	__m128 rasterX = _mm_mul_ps(uvX, _mm_cvtepi32_ps(width));
	__m128 rasterY = _mm_mul_ps(uvY, _mm_cvtepi32_ps(height));

	union { __m128i floorX; int floorX_arr[4]; };
	union { __m128i floorY; int floorY_arr[4]; };
	floorX = _mm_cvtps_epi32(_mm_floor_ps(rasterX));
	floorY = _mm_cvtps_epi32(_mm_floor_ps(rasterY));

	//t
	__m128 tX = _mm_sub_ps(rasterX, _mm_cvtepi32_ps(floorX));
	__m128 tY = _mm_sub_ps(rasterY, _mm_cvtepi32_ps(floorY));

	//1 - t
	__m128 oneF = _mm_set1_ps(1.f);
	__m128 tInvX = _mm_sub_ps(oneF, tX);
	__m128 tInvY = _mm_sub_ps(oneF, tY);

	//load colors
	//red
	union { __m128 ulR; float ulR_arr[4]; }; //upper-left
	union { __m128 urR; float urR_arr[4]; }; //upper-right
	union { __m128 llR; float llR_arr[4]; }; //lower-left
	union { __m128 lrR; float lrR_arr[4]; }; //lower-right

	//green
	union { __m128 ulG; float ulG_arr[4]; }; //upper-left
	union { __m128 urG; float urG_arr[4]; }; //upper-right
	union { __m128 llG; float llG_arr[4]; }; //lower-left
	union { __m128 lrG; float lrG_arr[4]; }; //lower-right

	//blue
	union { __m128 ulB; float ulB_arr[4]; }; //upper-left
	union { __m128 urB; float urB_arr[4]; }; //upper-right
	union { __m128 llB; float llB_arr[4]; }; //lower-left
	union { __m128 lrB; float lrB_arr[4]; }; //lower-right

	//load into registers
	for (int lane = 0; lane < 4; lane++) {
		cml::vec3 ul = (*this)[(size_t)floorY_arr[lane]+1][(size_t)floorX_arr[lane]];
		cml::vec3 ur = (*this)[(size_t)floorY_arr[lane]+1][(size_t)floorX_arr[lane]+1];
		cml::vec3 ll = (*this)[(size_t)floorY_arr[lane]][(size_t)floorX_arr[lane]];
		cml::vec3 lr = (*this)[(size_t)floorY_arr[lane]][(size_t)floorX_arr[lane]+1];

		ulR_arr[lane] = ul.x;
		urR_arr[lane] = ur.x;
		llR_arr[lane] = ll.x;
		lrR_arr[lane] = lr.x;

		ulG_arr[lane] = ul.y;
		urG_arr[lane] = ur.y;
		llG_arr[lane] = ll.y;
		lrG_arr[lane] = lr.y;

		ulB_arr[lane] = ul.z;
		urB_arr[lane] = ur.z;
		llB_arr[lane] = ll.z;
		lrB_arr[lane] = lr.z;
	}

	//upper interpolation
	__m128 upR = _mm_add_ps(_mm_mul_ps(ulR, tInvX), _mm_mul_ps(urR, tX));
	__m128 upG = _mm_add_ps(_mm_mul_ps(ulG, tInvX), _mm_mul_ps(urG, tX));
	__m128 upB = _mm_add_ps(_mm_mul_ps(ulB, tInvX), _mm_mul_ps(urB, tX));

	//lower interpolation
	__m128 lowR = _mm_add_ps(_mm_mul_ps(llR, tInvX), _mm_mul_ps(lrR, tX));
	__m128 lowG = _mm_add_ps(_mm_mul_ps(llG, tInvX), _mm_mul_ps(lrG, tX));
	__m128 lowB = _mm_add_ps(_mm_mul_ps(llB, tInvX), _mm_mul_ps(lrB, tX));

	//vertical interpolation
	ColorSIMD result;
	result.r = _mm_add_ps(_mm_mul_ps(lowR, tInvY), _mm_mul_ps(upR, tY));
	result.g = _mm_add_ps(_mm_mul_ps(lowG, tInvY), _mm_mul_ps(upG, tY));
	result.b = _mm_add_ps(_mm_mul_ps(lowB, tInvY), _mm_mul_ps(upB, tY));
	return result;
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