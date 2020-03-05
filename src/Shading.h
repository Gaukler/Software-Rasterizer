#pragma once
#include "pch.h"
#include "CML/Vector.h"
#include "Mesh.h"
#include "ShaderInput.h"
#include "ColorSIMD.h"

namespace ShadingFunctions {
	ColorSIMD normalVis(const InterpolationResult& v, const ShaderInput& input);
	ColorSIMD depthVis(const InterpolationResult& v, const ShaderInput& input);
	ColorSIMD texture(const InterpolationResult& v, const ShaderInput& input);
	ColorSIMD texturedLit(const InterpolationResult& v, const ShaderInput& input);
}
