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

size_t RenderTarget::getWidth() {
	return m_image->getWidth();
}

size_t RenderTarget::getHeight() {
	return m_image->getHeight();
}

RGBImage* RenderTarget::getImage() {
	return m_image;
}