#ifndef _PVRIMAGE_H_
#define _PVRIMAGE_H_

namespace Palleon
{
	enum PVR_PIXEL_FORMAT
	{
		PVR_PIXEL_OGL_PVRTC4 = 0x19
	};

	enum
	{
		PVR_FLAG_MIPMAP		= 0x0100,
		PVR_FLAG_TWIDDLED	= 0x0200,
		PVR_FLAG_CUBEMAP	= 0x1000,
	};

	enum
	{
		PVR_SIGNATURE = 0x21525650,		//PVR!
	};

	struct PVR_HEADER
	{
		uint32		headerSize;
		uint32		width;
		uint32		height;
		uint32		mipMapCount;
		uint32		flags;
		uint32		dataSize;
		uint32		bitCount;
		uint32		redMask;
		uint32		greenMask;
		uint32		blueMask;
		uint32		alphaMask;
		uint32		signature;
		uint32		surfaceCount;

		PVR_PIXEL_FORMAT GetPixelFormat() const
		{
			return static_cast<PVR_PIXEL_FORMAT>(flags & 0xFF);
		}

		bool IsValid() const
		{
			return (headerSize == sizeof(PVR_HEADER)) && (signature == PVR_SIGNATURE);
		}
	};

	static_assert(sizeof(PVR_HEADER) == 0x34, "PVR Header must be 0x34 bytes.");
}

#endif
