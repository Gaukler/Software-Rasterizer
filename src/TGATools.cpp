#include "pch.h"
#include "TGATools.h"

namespace TGATools {

	TGAImage::~TGAImage() {
		delete data;
	}

	TGAImage* loadTGAFile(const std::string& path) {

		std::ifstream file(path, std::ios::binary);
		assert(file.is_open());

		TGAheader header;
		file.read((char*)&header, sizeof(header));
		assert(image->header.imageType == TGAimageTypes::uncompressedRGB); //no other type supported

		const short bytePerPixel = (short)header.bitPerPixel >> 3;
		if (header.idLength != 0) std::cout << "ID field not supported, will be ignored " << path << std::endl;

		const size_t dataSize = (size_t)header.width * (size_t)header.height * (size_t)bytePerPixel;
		char* data = new char[dataSize];
		file.read(data, (std::streamsize)dataSize);
		return new TGAImage(header, bytePerPixel, data);
	}

	RGBImage* TGAtoRGBimage(TGAImage* tga) {
		RGBImage* rgb = new RGBImage((size_t)tga->header.width, (size_t)tga->header.height);
		assert(tga->bytePerPixel == 3); //only RGB support

		auto correctLoad = [](const float value) {
			if (value < 0.f) {
				return 255 + value;
			}
			return value;
		};

		for (int y = 0; y < tga->header.height; y++) {
			for (int x = 0; x < tga->header.width; x++) {
				const float blue   = correctLoad(tga->data[(y + x * tga->header.width) * tga->bytePerPixel]);
				const float green  = correctLoad(tga->data[(y + x * tga->header.width) * tga->bytePerPixel + 1]);
				const float red    = correctLoad(tga->data[(y + x * tga->header.width) * tga->bytePerPixel + 2]);
				(*rgb)[(size_t)x][(size_t)y] = cml::vec3(red, green, blue) / 255.f;
			}
		}
		return rgb;
	}

	TGAImage* RGBtoTGAImage(RGBImage* rgb) {
		TGAheader header;
		header.idLength = 0;
		header.colorMapType = 0;
		header.imageType = TGAimageTypes::uncompressedRGB;
		for (int i = 0; i < 5; i++) {
			header.colorMapSpecification[i] = 0;
		}
		header.xOrigin = 0;
		header.yOrigin = 0;
		header.width  = (short)rgb->getWidth();
		header.height = (short)rgb->getHeight();
		header.bitPerPixel = 24;
		header.imageDescriptorFlags = 0;

		const short bytesPerPixel = 3;
		char* data = new char[(size_t)header.width * (size_t)header.height * (size_t)bytesPerPixel];
		for (size_t y = 0; (short)y < header.height; y++) {
			for (size_t x = 0; (short)x < header.width; x++) {
				cml::vec3 pixel = (*rgb)[y][x];
				pixel.x = std::max(std::min(pixel.x, 1.f), 0.f);
				pixel.y = std::max(std::min(pixel.y, 1.f), 0.f);
				pixel.z = std::max(std::min(pixel.z, 1.f), 0.f);
				pixel = pixel * 255.f;
				const char red   = static_cast<char>(round(pixel.x));
				const char green = static_cast<char>(round(pixel.y));
				const char blue  = static_cast<char>(round(pixel.z));

				data[(x + y * header.width) * bytesPerPixel]     = blue;
				data[(x + y * header.width) * bytesPerPixel + 1] = green;
				data[(x + y * header.width) * bytesPerPixel + 2] = red;
			}
		}
		
		return new TGAImage(header, bytesPerPixel, data);
	}

	void writeTGAImage(TGAImage* tga, const std::string& filename) {
		std::ofstream file(filename, std::ios::binary);
		file.write((char*)&tga->header, sizeof(tga->header));
		file.write(tga->data, (std::streamsize)tga->header.width * (std::streamsize)tga->header.height * (std::streamsize)tga->bytePerPixel);
	}
}