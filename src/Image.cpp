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

ColorSIMD RGBImage::sampleSIMD(__m128 uvX, __m128 uvY) {
	//clamp uvs
	//not 0/1 because floor/ceil must not be max value
	__m128 min = _mm_set1_ps(0.001f); 
	__m128 max = _mm_set1_ps(0.999f); 

	uvX = _mm_min_ps(_mm_max_ps(uvX, min), max);
	uvY = _mm_min_ps(_mm_max_ps(uvY, min), max);


	//floor
	__m128i width  = _mm_set1_epi32(m_width  - 1);
	__m128i height = _mm_set1_epi32(m_height - 1);
	
	__m128 rasterX = _mm_mul_ps(uvX, _mm_cvtepi32_ps(width));
	__m128 rasterY = _mm_mul_ps(uvY, _mm_cvtepi32_ps(height));

	union { __m128i floorX; int floorX_arr[4]; };
	union { __m128i floorY; int floorY_arr[4]; };
	floorX = _mm_cvtps_epi32(_mm_floor_ps(rasterX));
	floorY = _mm_cvtps_epi32(_mm_floor_ps(rasterY));

	//ceil
	union { __m128i ceilX; int ceilX_arr[4]; };
	union { __m128i ceilY; int ceilY_arr[4]; };

	__m128i oneI = _mm_set1_epi32(1);
	ceilX = _mm_add_epi32(floorX, oneI);
	ceilY = _mm_add_epi32(floorY, oneI);

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
		cml::vec3 ul = (*this)[(size_t)ceilY_arr[lane]][(size_t)floorX_arr[lane]];
		cml::vec3 ur = (*this)[(size_t)ceilY_arr[lane]][(size_t)ceilX_arr[lane]];
		cml::vec3 ll = (*this)[(size_t)floorY_arr[lane]][(size_t)floorX_arr[lane]];
		cml::vec3 lr = (*this)[(size_t)floorY_arr[lane]][(size_t)ceilX_arr[lane]];

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