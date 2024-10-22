#pragma once
#include "pch.h"
#include "Image.h"

namespace TGATools {

	enum class TGAimageTypes : char {
		noImage = 0,
		uncompressedColorMapped = 1,
		uncompressedRGB = 2,
		uncompressedBlackWhite = 3,
		runlengthEncodedColorMapped = 9,
		runlengthEncodedRGB = 10,
		compressedBlackWhite = 11,
		compressedColorMappedVar1 = 32,
		compressedColorMappedVar2 = 33
	};

	//no constructor as it's written to directly when loading a TGA file
	struct TGAheader {
		char idLength;
		char colorMapType;
		TGAimageTypes imageType;
		char colorMapSpecification[5];
		short xOrigin;
		short yOrigin;
		short width;
		short height;
		char bitPerPixel;
		char imageDescriptorFlags;
	};

	struct TGAImage {
		inline TGAImage(const TGAheader header, const short bytePerPixel, char* data)
			: header(header), bytePerPixel(bytePerPixel), data(data){};
		~TGAImage();
		TGAheader header;
		short bytePerPixel;
	private:
		float padding = 0.f;
	public:
		char* data;
	};

	TGAImage* loadTGAFile(const std::string& path);
	void writeTGAImage(TGAImage* tga, const std::string& filename);

	RGBImage* TGAtoRGBimage(TGAImage* tga);
	TGAImage* RGBtoTGAImage(RGBImage* rgb);
}