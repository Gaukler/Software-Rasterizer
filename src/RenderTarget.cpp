#pragma once
#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(const size_t width, const size_t height) {
	m_image = new RGBImage(width, height);
	m_image->clear();

	m_depthBuffer = new IntImage(width, height);
	m_depthBuffer->clear(-INT_MAX);
}

RenderTarget::~RenderTarget() {
	delete m_image;
}

cml::vec3 RenderTarget::read(const size_t x, const size_t y) {
	return (*m_image)[y][x];
}
void RenderTarget::write(const size_t x, const size_t y, const cml::vec3& value) {
	(*m_image)[y][x] = value;
}
void RenderTarget::writeDepthTest(const size_t x, const size_t y, const cml::vec3& value, const int& depth) {
	if (depth < (*m_depthBuffer)[y][x]){
		return;
	}
	(*m_image)[y][x] = value;
	(*m_depthBuffer)[y][x] = depth;
}

void RenderTarget::writeDepthTestSIMD(int x[4], const size_t y, const ColorSIMD& colors, const __m128i& valid, const int depth_arr[4]) {

	__m128i depth = _mm_setr_epi32(depth_arr[0], depth_arr[1], depth_arr[2], depth_arr[3]);

	union { __m128i depthImage; int depthImage_arr[4]; };
	for (int lane = 0; lane < 4; lane++) {
		depthImage_arr[lane] = (*m_depthBuffer)[y][x[lane]];
	}

	union { __m128i drawPixel; int drawPixel_arr[4]; };
	drawPixel = _mm_and_si128(_mm_cmpgt_epi32(depth, depthImage), valid);

	union { __m128i xReg; int x_arr[4]; };
	union { __m128i yReg; int y_arr[4]; };
	yReg = _mm_set1_epi32(y);
	xReg = _mm_setr_epi32(x[0], x[1], x[2], x[3]);

	xReg = _mm_and_si128(xReg, drawPixel);
	yReg = _mm_and_si128(yReg, drawPixel);

	for (int lane = 0; lane < 4; lane++) {
		if (drawPixel_arr[lane]) {
			cml::vec3 pixel = cml::vec3(colors.rArr[lane], colors.gArr[lane], colors.bArr[lane]);
			(*m_image)[y][x[lane]] = pixel;
			(*m_depthBuffer)[y][x[lane]] = depth_arr[lane];
		}
	}
}

size_t RenderTarget::getWidth() {
	return m_image->getWidth();
}

size_t RenderTarget::getHeight() {
	return m_image->getHeight();
}

RGBImage* RenderTarget::getImage() {
	return m_image;
}