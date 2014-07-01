#pragma once

#include <stdint.h>

namespace bmp
{

#pragma pack(push, 1)

typedef struct _BITMAP_FILEHEADER {
	uint16_t Signature;
	uint32_t Size;
	uint32_t Reserved;
	uint32_t BitsOffset;
} BITMAP_FILEHEADER;

#define BITMAP_FILEHEADER_SIZE 14

typedef struct _BITMAP_HEADER {
	uint32_t HeaderSize;
	int32_t Width;
	int32_t Height;
	uint16_t Planes;
	uint16_t BitCount;
	uint32_t Compression;
	uint32_t SizeImage;
	int32_t PelsPerMeterX;
	int32_t PelsPerMeterY;
	uint32_t ClrUsed;
	uint32_t ClrImportant;
	uint32_t RedMask;
	uint32_t GreenMask;
	uint32_t BlueMask;
	uint32_t AlphaMask;
	uint32_t CsType;
	uint32_t Endpoints[9]; 
	uint32_t GammaRed;
	uint32_t GammaGreen;
	uint32_t GammaBlue;
} BITMAP_HEADER;

#pragma pack(pop)

void write_32bit_header(std::ostream & os, int width, int height)
{
  BITMAP_FILEHEADER bfh;
	BITMAP_HEADER bh;
	memset(&bfh, 0, sizeof(bfh));
	memset(&bh, 0, sizeof(bh));

	bfh.Signature = 0x4d42;
	bfh.BitsOffset = sizeof(BITMAP_FILEHEADER) + sizeof(BITMAP_HEADER);
	bfh.Size = width * height * 4 + bfh.BitsOffset;
	
	bh.HeaderSize = sizeof(BITMAP_HEADER);
	bh.BitCount = 32;
		
	bh.Compression = 3; 
	bh.AlphaMask = 0xff000000;
	bh.BlueMask = 0x00ff0000;
	bh.GreenMask = 0x0000ff00;
	bh.RedMask = 0x000000ff;

  bh.Planes = 1;
	bh.Height = -height; // Top-to-bottom lines order
	bh.Width = width;
	bh.SizeImage = width * 4 * height;
	bh.PelsPerMeterX = 3780;
	bh.PelsPerMeterY = 3780;
		
  // TODO: flip bits on big-endian machine
	os.write((char*) &bfh, sizeof(BITMAP_FILEHEADER));
	os.write((char*) &bh, sizeof(BITMAP_HEADER));
}

}