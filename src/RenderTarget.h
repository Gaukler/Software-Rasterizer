#pragma once
#include "pch.h"
#include "CML/Vector.h"
#include "Image.h"

class RenderTarget {
public:
	RenderTarget(const size_t width, const size_t height);
	~RenderTarget();
	cml::vec3 read(const size_t x, const size_t y);
	void write(const size_t x, const size_t y, const cml::vec3& value);
	void writeDepthTest(const size_t x, const size_t y, const cml::vec3& value, const int& depth);
	RGBImage* getImage();

	size_t getWidth();
	size_t getHeight();
private:
	RGBImage* m_image;
	IntImage* m_depthBuffer;
};