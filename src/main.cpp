#include "pch.h"
#include "TGATools.h"
#include "CML/Vector.h"
#include "CML/Common.h"
#include "RenderTarget.h"
#include "Rasterization.h"
#include "objLoader.h"
#include "Shading.h"
#include "VertexShader.h"
#include "CML/MatrixTransforms.h"

int main() {
	int width = 1920;
	int height = 1080;
	RenderTarget renderTarget(width, height);

	std::string modelName = "C:/Users/Nutzer/Desktop/african_head.obj";
	Mesh* mesh = objLoader::loadOBJ(modelName);

	TGATools::TGAImage* textureTGA = TGATools::loadTGAFile("C:/Users/Nutzer/Desktop/african_head_diffuse.tga");
	RGBImage* texture = TGATools::TGAtoRGBimage(textureTGA);
	ShaderInput shaderInput;
	shaderInput.texture = texture;
	float fovY = 40.f;
	float aspectRatio = (float)width / height;
	shaderInput.projectionMatrix = cml::projectionMatrix(1.f, 20.f, cml::radian(fovY), aspectRatio);
	shaderInput.viewMatrix = cml::viewMatrixLookAt(cml::vec3(1.f, 1.f, 3.f), cml::vec3(0.f));

	shaderInput.lightPosition = cml::vec3(1.f, 1.f, 1.f);

	RenderSettings settings;
	settings.rasterType = RasterizationType::Fill;
	settings.vertexFunctions = VertexFunctions::viewProjection;
	settings.shadingFunction = ShadingFunctions::texturedLit;
	settings.shaderInput = shaderInput;
	drawTriangles(mesh->triangles, renderTarget, settings);

	TGATools::TGAImage* tgaImage = TGATools::RGBtoTGAImage(renderTarget.getImage());
	std::string filename = "C:/Users/Nutzer/Desktop/testImage.tga";
	TGATools::writeTGAImage(tgaImage, filename);

	return 0;
}