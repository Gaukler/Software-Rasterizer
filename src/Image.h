#pragma once
#include "pch.h"
#include "CML/Vector.h"
#include "ColorSIMD.h"

struct RGBImageRow {
	RGBImageRow(size_t length);
	~RGBImageRow();
	cml::vec3& operator [](size_t index);
private:
	cml::vec3* m_data;
};

class RGBImage {
public:
	RGBImage(size_t width, size_t height);
	size_t getWidth();
	size_t getHeight();
	void clear(const cml::vec3& color = cml::vec3(0.f));
	ColorSIMD sampleSIMD(__m128 uvX, __m128 uvY);

	RGBImageRow& operator [](size_t index);
	~RGBImage();
private:
	size_t m_width;
	size_t m_height;
	RGBImageRow* m_data;
};

struct IntImageRow {
	IntImageRow(size_t length);
	~IntImageRow();
	int& operator [](size_t index);
private:
	int* m_data;
};

class IntImage {
public:
	IntImage(size_t width, size_t height);
	size_t getWidth();
	size_t getHeight();
	void clear(const int& value = INT_MAX);

	IntImageRow& operator [](size_t index);
	~IntImage();
private:
	size_t m_width;
	size_t m_height;
	IntImageRow* m_data;
};