#include "pch.h"
#include "TGATools.h"

namespace TGATools {

	TGAImage::~TGAImage() {
		delete data;
	}

	TGAImage* loadTGAFile(const std::string& path) {

		TGAImage* image = new TGAImage;

		std::ifstream file(path, std::ios::binary);
		if (!file.is_open()) {
			std::cout << "Unable to open file " << path << std::endl;
			return nullptr;
		}
		file.read((char*)&image->header, sizeof(image->header));
		if (image->header.imageType != TGAimageTypes::uncompressedRGB) {
			std::cout << "Unsupported TGA type " << path << std::endl;
			return nullptr;
		}
		short idFieldLength = (short)image->header.idLength;
		if (idFieldLength != 0) {
			std::cout << "ID field not supported " << path << std::endl;
		}
		image->bytePerPixel = (short)image->header.bitPerPixel>>3;
		unsigned long dataSize = image->header.width * image->header.height * image->bytePerPixel;
		image->data = new char[dataSize];
		file.read(image->data, dataSize);
		return image;
	}

	float correctLoad(float value) {
		if (value < 0.f) {
			return 255 + value;
		}
		return value;
	}

	RGBImage* TGAtoRGBimage(TGAImage* tga) {
		RGBImage* rgb = new RGBImage(tga->header.width, tga->header.height);
		if (tga->bytePerPixel != 3) {
			std::cout << "Conversion only supports RGB" << std::endl;
			return nullptr;
		}
		for (int y = 0; y < tga->header.height; y++) {
			for (int x = 0; x < tga->header.width; x++) {
				float blue   = correctLoad(tga->data[(y + x * tga->header.width) * tga->bytePerPixel]);
				float green  = correctLoad(tga->data[(y + x * tga->header.width) * tga->bytePerPixel + 1]);
				float red    = correctLoad(tga->data[(y + x * tga->header.width) * tga->bytePerPixel + 2]);
				(*rgb)[x][y] = cml::vec3(red, green, blue) / 255.f;
			}
		}
		return rgb;
	}

	TGAImage* RGBtoTGAImage(RGBImage* rgb) {
		TGAImage* tga = new TGAImage;
		tga->bytePerPixel = 3;

		tga->header.idLength = 0;
		tga->header.colorMapType = 0;
		tga->header.imageType = TGAimageTypes::uncompressedRGB;
		for (int i = 0; i < 5; i++) {
			tga->header.colorMapSpecification[i] = 0;
		}
		tga->header.xOrigin = 0;
		tga->header.yOrigin = 0;
		tga->header.width = rgb->getWidth();
		tga->header.height = rgb->getHeight();
		tga->header.bitPerPixel = 24;
		tga->header.imageDescriptorFlags = 0;

		tga->data = new char[tga->header.width * tga->header.height * tga->bytePerPixel];
		for (int y = 0; y < tga->header.height; y++) {
			for (int x = 0; x < tga->header.width; x++) {
				cml::vec3 pixel = (*rgb)[y][x];
				pixel.x = std::max(std::min(pixel.x, 1.f), 0.f);
				pixel.y = std::max(std::min(pixel.y, 1.f), 0.f);
				pixel.z = std::max(std::min(pixel.z, 1.f), 0.f);
				pixel = pixel * 255.f;
				char red   = static_cast<char>(round(pixel.x));
				char green = static_cast<char>(round(pixel.y));
				char blue  = static_cast<char>(round(pixel.z));

				tga->data[(x + y * tga->header.width) * tga->bytePerPixel]     = blue;
				tga->data[(x + y * tga->header.width) * tga->bytePerPixel + 1] = green;
				tga->data[(x + y * tga->header.width) * tga->bytePerPixel + 2] = red;
			}
		}

		return tga;
	}

	void writeTGAImage(TGAImage* tga, const std::string& filename) {
		std::ofstream file(filename, std::ios::binary);
		file.write((char*)&tga->header, sizeof(tga->header));
		file.write(tga->data, tga->header.width * tga->header.height * tga->bytePerPixel);
	}
}