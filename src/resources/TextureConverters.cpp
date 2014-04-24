#include "palleon/resources/TextureConverters.h"

using namespace Palleon;

static void DecodeDxt1Block(uint8* dstData, size_t dstPitch, const uint8* srcData)
{
	static const size_t pixelSize = 3;
	
	uint32 c0 = srcData[0] | (srcData[1] << 8);
	uint32 c1 = srcData[2] | (srcData[3] << 8);
	uint32 bits = srcData[4] | (srcData[5] << 8) | (srcData[6] << 16) | (srcData[7] << 24);
	
	uint32 b0 = (c0 & 0x1f) << 3;
	uint32 g0 = ((c0 >> 5) & 0x3f) << 2;
	uint32 r0 = ((c0 >> 11) & 0x1f) << 3;
	
	uint32 b1 = (c1 & 0x1f) << 3;
	uint32 g1 = ((c1 >> 5) & 0x3f) << 2;
	uint32 r1 = ((c1 >> 11) & 0x1f) << 3;
	
	for(unsigned int y = 0; y < 4; y++)
	{
		uint8* dstLinePtr = dstData + (y * dstPitch);
		for(unsigned int x = 0; x < 4; x++)
		{
			uint32 control = bits & 0x03;
			bits >>= 2;
			switch(control)
			{
				case 0:
					dstLinePtr[0] = r0;
					dstLinePtr[1] = g0;
					dstLinePtr[2] = b0;
					break;
				case 1:
					dstLinePtr[0] = r1;
					dstLinePtr[1] = g1;
					dstLinePtr[2] = b1;
					break;
				case 2:
					if(c0 > c1)
					{
						dstLinePtr[0] = (2 * r0 + r1) / 3;
						dstLinePtr[1] = (2 * g0 + g1) / 3;
						dstLinePtr[2] = (2 * b0 + b1) / 3;
					}
					else
					{
						dstLinePtr[0] = (r0 + r1) / 2;
						dstLinePtr[1] = (g0 + g1) / 2;
						dstLinePtr[2] = (b0 + b1) / 2;
					}
					break;
				case 3:
					if(c0 > c1)
					{
						dstLinePtr[0] = (r0 + 2 * r1) / 3;
						dstLinePtr[1] = (g0 + 2 * g1) / 3;
						dstLinePtr[2] = (b0 + 2 * b1) / 3;
					}
					else
					{
						dstLinePtr[0] = 0;
						dstLinePtr[1] = 0;
						dstLinePtr[2] = 0;
					}
					break;
			}
			dstLinePtr += pixelSize;
		}
	}
}

std::vector<uint8> CTextureConverters::Dxt1ToRgb(uint32 width, uint32 height, const void* data)
{
	static const size_t pixelSize = 3;
	std::vector<uint8> dstData(width * height * pixelSize);
	const uint8* srcData = reinterpret_cast<const uint8*>(data);
	const size_t dstPitch = width * pixelSize;
	for(unsigned int y = 0; y < height; y += 4)
	{
		uint8* dstLinePtr = dstData.data() + (dstPitch * y);
		for(unsigned int x = 0; x < width; x += 4)
		{
			DecodeDxt1Block(dstLinePtr, dstPitch, srcData);
			dstLinePtr += (pixelSize * 4);
			srcData += 8;
		}
	}
	return dstData;
}

std::vector<uint8> CTextureConverters::BgraToRgba(uint32 width, uint32 height, const void* data)
{
	uint32 pixelCount = width * height;
	static const size_t pixelSize = 4;
	std::vector<uint8> dstData(pixelCount * pixelSize);
	for(uint32 i = 0; i < pixelCount; i++)
	{
		const uint8* srcPixel = reinterpret_cast<const uint8*>(data) + (i * pixelSize);
		uint8* dstPixel = dstData.data() + (i * pixelSize);
		dstPixel[0] = srcPixel[2];
		dstPixel[1] = srcPixel[1];
		dstPixel[2] = srcPixel[0];
		dstPixel[3] = srcPixel[3];
	}
	return dstData;
}
