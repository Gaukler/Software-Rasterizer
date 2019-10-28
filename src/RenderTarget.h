#pragma once
#include "pch.h"
#include "CML/Vector.h"
#include "Image.h"

class RenderTarget {
public:
	RenderTarget(const int width, const int height);
	~RenderTarget();
	cml::vec3 read(const int x, const int y);
	void write(const int x, const int y, const cml::vec3& value);
	void writeDepthTest(const int x, const int y, const cml::vec3& value, const int& depth);
	RGBImage* getImage();

	int getWidth();
	int getHeight();
private:
	RGBImage* m_image;
	IntImage* m_depthBuffer;
};