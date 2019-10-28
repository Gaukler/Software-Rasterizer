#pragma once
#include "pch.h"
#include "CML/Vector.h"

struct RGBImageRow {
	RGBImageRow(int length);
	~RGBImageRow();
	cml::vec3& operator [](int index);
private:
	cml::vec3* m_data;
};

class RGBImage {
public:
	RGBImage(int width, int height);
	int getWidth();
	int getHeight();
	void clear(const cml::vec3& color = cml::vec3(0.f));
	cml::vec3 sample(const cml::vec2& uv);

	RGBImageRow& operator [](int index);
	~RGBImage();
private:
	int m_width;
	int m_height;
	RGBImageRow* m_data;
};

struct IntImageRow {
	IntImageRow(int length);
	~IntImageRow();
	int& operator [](int index);
private:
	int* m_data;
};

class IntImage {
public:
	IntImage(int width, int height);
	int getWidth();
	int getHeight();
	void clear(const int& value = INT_MAX);

	IntImageRow& operator [](int index);
	~IntImage();
private:
	int m_width;
	int m_height;
	IntImageRow* m_data;
};