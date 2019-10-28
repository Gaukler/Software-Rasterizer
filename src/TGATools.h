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
		short bytePerPixel;
		TGAheader header;
		char* data;
		~TGAImage();
	};

	TGAImage* loadTGAFile(const std::string& path);
	void writeTGAImage(TGAImage* tga, const std::string& filename);

	RGBImage* TGAtoRGBimage(TGAImage* tga);
	TGAImage* RGBtoTGAImage(RGBImage* rgb);
}