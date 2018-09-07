#include "rgbimage.h"
#include "color.h"
#include "assert.h"
#include "matrix.h"

#include "Windows.h"

//#define DEBUG

#pragma region Constructors

RGBImage::RGBImage(unsigned int Width, unsigned int Height)
{
	this->m_Width = Width;
	this->m_Height = Height;
	this->m_Image = new Color[Width * Height]();
}

RGBImage::~RGBImage()
{
	delete[] m_Image;
}

#pragma endregion

#pragma region Methods

void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color& c)
{
	// Test if coordinates lie within image buffer
	if (x > this->width() || y > this->height())
		throw std::out_of_range("Exception: XY coordinates lie outside of the image buffer!\n");

	// this->width() * y results in the "row" of the array. Add x to it and you get the y position of the pixel in question
	this->m_Image[this->width() * y + x] = c;
}

const Color& RGBImage::getPixelColor(unsigned int x, unsigned int y) const
{
	if (x >= this->width() || y >= this->height())
		throw std::out_of_range("Exception: XY coordinates lie outside of the image buffer!\n");

	return this->m_Image[this->width() * y + x];
}

unsigned int RGBImage::width() const
{
	return this->m_Width;
}

unsigned int RGBImage::height() const
{
	return this->m_Height;
}

unsigned char RGBImage::convertColorChannel(float v)
{
	v = Color::clampColorRange(v);
	return (unsigned char)(v * 255);
}

bool RGBImage::saveToDisk(const char* Filename)
{
	// BITMAPFILEHEADER reference (MSDN): https://msdn.microsoft.com/en-us/library/dd183374(v=vs.85).aspx
	/*
	typedef struct tagBITMAPFILEHEADER {
		WORD  bfType;
		DWORD bfSize;
		WORD  bfReserved1;
		WORD  bfReserved2;
		DWORD bfOffBits;
	} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

	bfType
	The file type; must be BM.

	bfSize
	The size, in bytes, of the bitmap file.

	bfReserved1
	Reserved; must be zero.

	bfReserved2
	Reserved; must be zero.

	bfOffBits
	The offset, in bytes, from the beginning of the BITMAPFILEHEADER structure to the bitmap bits.
	*/

	BITMAPFILEHEADER* bmpHeader = new BITMAPFILEHEADER();
	bmpHeader->bfType = 0x4d42;
	bmpHeader->bfReserved1 = 0;
	bmpHeader->bfReserved2 = 0;
	bmpHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpHeader->bfSize = bmpHeader->bfOffBits + (DWORD)(this->width() * this->height() * 3); // times 3 to save each channel

	// BITMAPINFOHEADER reference (MSDN): https://msdn.microsoft.com/en-us/library/dd183376(v=vs.85).aspx
	/*
	typedef struct tagBITMAPINFOHEADER {
		DWORD biSize;
		LONG  biWidth;
		LONG  biHeight;
		WORD  biPlanes;
		WORD  biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		LONG  biXPelsPerMeter;
		LONG  biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
	} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

	biSize
	The number of bytes required by the structure.

	biWidth
	The width of the bitmap, in pixels.

	If biCompression is BI_JPEG or BI_PNG, the biWidth member specifies the width of the decompressed JPEG or PNG image file, respectively.

	biHeight
	The height of the bitmap, in pixels. If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner. If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner.

	If biHeight is negative, indicating a top-down DIB, biCompression must be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be compressed.

	If biCompression is BI_JPEG or BI_PNG, the biHeight member specifies the height of the decompressed JPEG or PNG image file, respectively.

	biPlanes
	The number of planes for the target device. This value must be set to 1.

	biBitCount
	The number of bits-per-pixel. The biBitCount member of the BITMAPINFOHEADER structure determines the number of bits that define each pixel and the maximum number of colors in the bitmap. This member must be one of the following values.
	*/

	BITMAPINFOHEADER* bmpInfo = new BITMAPINFOHEADER();
	bmpInfo->biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo->biWidth = this->width();
	bmpInfo->biHeight = this->height() * -1; // flip image
	bmpInfo->biPlanes = 1;
	bmpInfo->biBitCount = 24;
	bmpInfo->biCompression = BI_RGB;
	bmpInfo->biSizeImage = 0;
	bmpInfo->biXPelsPerMeter = 0;
	bmpInfo->biYPelsPerMeter = 0;
	bmpInfo->biClrUsed = 0;
	bmpInfo->biClrImportant = 0;

	FILE* file;

	// Try open as binary file
	try {
		file = fopen(Filename, "wb");
	}
	catch (const std::exception& e) {
		printf("Exception: Couldn't open file!\n");
		printf(e.what());
	}

	if (!file) throw std::exception("Exception: Couldn't create file!\n");
	fwrite(bmpHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fwrite(bmpInfo, sizeof(BITMAPINFOHEADER), 1, file);

	for (unsigned int y = 0; y < this->height(); ++y)
	{
		for (unsigned int x = 0; x < this->width(); ++x)
		{
			Color pixelColor = this->getPixelColor(x, y);
			unsigned char r, g, b;
			r = this->convertColorChannel(pixelColor.R);
			g = this->convertColorChannel(pixelColor.G);
			b = this->convertColorChannel(pixelColor.B);

			fwrite(&b, sizeof(char), 1, file);
			fwrite(&g, sizeof(char), 1, file);
			fwrite(&r, sizeof(char), 1, file);

			//unsigned char bgr[3];
			//bgr[0] = this->convertColorChannel(pixelColor.B);
			//bgr[1] = this->convertColorChannel(pixelColor.G);
			//bgr[2] = this->convertColorChannel(pixelColor.R);

			//fwrite(&bgr, sizeof(char)*3, 1, file);
		}
	}

	fclose(file);

	return true;
}

//dst - Ausgangsild
//src - Eingangsbild
//factor mit S multiplizieren
RGBImage& RGBImage::SobelFilter(RGBImage& mixMap, const RGBImage& heightMap, float factor)
{
	// check for bad values
	assert(heightMap.width() == mixMap.width() && heightMap.height() == mixMap.height());

	// filter mask
	Matrix k = Matrix();
	k.m00 = 1; k.m10 = 0; k.m20 = -1;
	k.m01 = 2; k.m11 = 0; k.m21 = -2;
	k.m02 = 1; k.m12 = 0; k.m22 = -1;

	// doesn't work... why?
	// transposed filter mask
	Matrix kT = k.transpose();

	unsigned int width = heightMap.width();
	unsigned int height = heightMap.height();

	RGBImage U = RGBImage(width, height);
	RGBImage V = RGBImage(width, height);

	// calculate mixmap values
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			for (int j = 0; j <= 2; ++j)
			{
				for (int i = 0; i <= 2; ++i)
				{
					int pixelX = x - i - 1;
					int pixelY = y - j - 1;

					// calculate U
					float kX = k.m[i + j * 4];

					// check if pixelX or pixelY below 0
					// if so, return new color, else get the color of the pixel
					Color heightMapColor = (pixelX < 0 || pixelY < 0) ? Color(0, 0, 0) : heightMap.getPixelColor(pixelX, pixelY);
					Color UColor = U.getPixelColor(x, y);
					UColor = Color((heightMapColor.R * kX) + UColor.R, (heightMapColor.G * kX) + UColor.G, (heightMapColor.B * kX) + UColor.B);

					U.setPixelColor(x, y, UColor);
				}
			}
		}
	}

	// transpose filter mask matrix
	k.transpose();

	// calculate mixmap values
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			for (int j = 0; j <= 2; ++j)
			{
				for (int i = 0; i <= 2; ++i)
				{
					int pixelX = x - i - 1;
					int pixelY = y - j - 1;

					// calculate V with transposed matrix
					float kY = k.m[i + j * 4];

					// check if pixelX or pixelY below 0
					// if so, return new color, else get the color of the pixel
					Color heightMapColor = (pixelX < 0 || pixelY < 0) ? Color(0, 0, 0) : heightMap.getPixelColor(pixelX, pixelY);
					Color VColor = V.getPixelColor(x, y);
					VColor = Color((heightMapColor.R * kY) + VColor.R, (heightMapColor.G * kY) + VColor.G, (heightMapColor.B * kY) + VColor.B);

					V.setPixelColor(x, y, VColor);
				}
			}
		}
	}

	// set mixmap colors
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			// raise all color values to the power of 2
			Color UColor = U.getPixelColor(x, y);
			UColor.R = powf(UColor.R, 2.0f);
			UColor.G = powf(UColor.G, 2.0f);
			UColor.B = powf(UColor.B, 2.0f);

			Color VColor = V.getPixelColor(x, y);
			VColor.R = powf(VColor.R, 2.0f);
			VColor.G = powf(VColor.G, 2.0f);
			VColor.B = powf(VColor.B, 2.0f);

			// then, take the square root from all color values
			Color UVColor = Color(sqrt(UColor.R + VColor.R), sqrt(UColor.G + VColor.G), sqrt(UColor.B + VColor.B));

			// multiply the color by a predefined factor
			UVColor *= factor;

			// set the pixel color
			mixMap.setPixelColor(x, y, UVColor);
		}
	}

	mixMap.saveToDisk("../../assets/mixMap2.bmp");

	return mixMap;
}

RGBImage & RGBImage::GaussFilter(RGBImage & gaussMap, const RGBImage & heightMap, float factor)
{
	return gaussMap;
}

#pragma endregion
