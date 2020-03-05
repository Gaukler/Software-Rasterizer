#include "pch.h"
#include "TGATools.h"
#include "CML/Vector.h"
#include "CML/Common.h"
#include "RenderTarget.h"
#include "RasterizationPublic.h"
#include "RenderSettings.h"
#include "objLoader.h"
#include "Shading.h"
#include "VertexShader.h"
#include "CML/MatrixTransforms.h"
#include "DirectoryUtils.h"
#include "Benchmark.h"

int main() {

	const uint32_t width = 1920;
	const uint32_t height = 1080;
	RenderTarget renderTarget(width, height);

	const std::filesystem::path resourcePath = DirectoryUtils::searchResourceDirectory();

	const std::string modelName = "african_head.obj";
	const std::filesystem::path modelPath = resourcePath / modelName;
	const Mesh* mesh = objLoader::loadOBJ(modelPath.string());

	const std::string textureName = "african_head_diffuse.tga";
	const std::filesystem::path texturePath = resourcePath / textureName;
	TGATools::TGAImage* textureSource = TGATools::loadTGAFile(texturePath.string());
	RGBImage* texture = TGATools::TGAtoRGBimage(textureSource);

	const cml::vec3 lightPosition = cml::vec3(1.f, 1.f, 1.f);
	const float fovY = 40.f;
	const float aspectRatio = (float)width / (float)height;
	const cml::mat4x4 projectionMatrix = cml::projectionMatrix(1.f, 20.f, cml::radian(fovY), aspectRatio);
	const cml::mat4x4 viewMatrix = cml::viewMatrixLookAt(cml::vec3(1.f, 1.f, 3.f), cml::vec3(0.f));

	const ShaderInput shaderInput(texture, lightPosition, projectionMatrix, viewMatrix);
	const RenderSettings settings(ShadingFunctions::texturedLit, VertexFunctions::viewProjection, shaderInput);

	const bool benchmarkRun = true;
	if (benchmarkRun) {
		benchmark(mesh, settings, renderTarget);
	}
	else {
		drawTriangles(mesh->triangles, renderTarget, settings);
	}

	TGATools::TGAImage* tgaImage = TGATools::RGBtoTGAImage(renderTarget.getImage());
	const std::string resultFileName = "render.tga";
	const std::filesystem::path resultPath = resourcePath / resultFileName;
	TGATools::writeTGAImage(tgaImage, resultPath.string());

	return 0;
}