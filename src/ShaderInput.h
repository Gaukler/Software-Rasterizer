#pragma once
#include "pch.h"
#include "Image.h"
#include "CML/Matrix.h"

struct ShaderInput {
	RGBImage* texture;
	cml::vec3 lightPosition;
	mutable cml::mat4x4 projectionMatrix;
	mutable cml::mat4x4 viewMatrix;
};
