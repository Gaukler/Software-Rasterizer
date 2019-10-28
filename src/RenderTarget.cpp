#pragma once
#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(const int width, const int height) {
	m_image = new RGBImage(width, height);
	m_image->clear();

	m_depthBuffer = new IntImage(width, height);
	m_depthBuffer->clear(-INT_MAX);
}

RenderTarget::~RenderTarget() {
	delete m_image;
}

cml::vec3 RenderTarget::read(const int x, const int y) {
	return (*m_image)[y][x];
}
void RenderTarget::write(const int x, const int y, const cml::vec3& value) {
	(*m_image)[y][x] = value;
}
void RenderTarget::writeDepthTest(const int x, const int y, const cml::vec3& value, const int& depth) {
	if (depth < (*m_depthBuffer)[y][x]){
		return;
	}
	(*m_image)[y][x] = value;
	(*m_depthBuffer)[y][x] = depth;
}

int RenderTarget::getWidth() {
	return m_image->getWidth();
}

int RenderTarget::getHeight() {
	return m_image->getHeight();
}

RGBImage* RenderTarget::getImage() {
	return m_image;
}