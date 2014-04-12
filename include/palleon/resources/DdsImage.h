#pragma once

#include <vector>
#include "Stream.h"

namespace Athena
{
	class CDdsImage
	{
	public:
		typedef std::vector<uint8> SurfaceByteArray;
		typedef std::vector<SurfaceByteArray> SurfaceArray;

								CDdsImage(Framework::CStream&);
		virtual					~CDdsImage();

		bool					IsCubeMap() const;
		
		uint32					GetWidth() const;
		uint32					GetHeight() const;
		uint32					GetPitch() const;

		const SurfaceArray&		GetSurfaces() const;

	private:
		void					Read(Framework::CStream&);

		struct DDS_PIXELFORMAT
		{
			uint32			size;
			uint32			flags;
			uint32			fourCC;
			uint32			rgbBitCount;
			uint32			rMask;
			uint32			gMask;
			uint32			bMask;
			uint32			aMask;
		};
		static_assert(sizeof(DDS_PIXELFORMAT) == 32, "Size of DDS_PIXELFORMAT must be 32 bytes.");

		enum DDS_PIXELFORMAT_FLAGS
		{
			DDS_PIXELFORMAT_FLAG_ALPHAPIXELS	= 0x00001,
			DDS_PIXELFORMAT_FLAG_ALPHA			= 0x00002,
			DDS_PIXELFORMAT_FLAG_FOURCC			= 0x00004,
			DDS_PIXELFORMAT_FLAG_RGB			= 0x00040,
			DDS_PIXELFORMAT_FLAG_YUV			= 0x00200,
			DDS_PIXELFORMAT_FLAG_LUMINANCE		= 0x20000
		};

		struct DDS_HEADER
		{
			uint32			size;
			uint32			flags;
			uint32			height;
			uint32			width;
			uint32			pitch;
			uint32			depth;
			uint32			mipMapCount;
			uint32			reserved1[11];
			DDS_PIXELFORMAT	ddspf;
			uint32			caps;
			uint32			caps2;
			uint32			caps3;
			uint32			caps4;
			uint32			reserved2;
		};
		static_assert(sizeof(DDS_HEADER) == 124, "Size of DDS_HEADER must be 124 bytes.");

		enum DDS_HEADER_FLAGS
		{
			DDS_HEADER_FLAG_CAPS			= 0x00000001,
			DDS_HEADER_FLAG_HEIGHT			= 0x00000002,
			DDS_HEADER_FLAG_WIDTH			= 0x00000004,
			DDS_HEADER_FLAG_PITCH			= 0x00000008,
			DDS_HEADER_FLAG_PIXELFORMAT		= 0x00001000,
			DDS_HEADER_FLAG_MIPMAPCOUNT		= 0x00020000,
			DDS_HEADER_FLAG_LINEARSIZE		= 0x00080000,
			DDS_HEADER_FLAG_DEPTH			= 0x00800000
		};

		enum DDS_HEADER_CAPS
		{
			DDS_HEADER_CAP_COMPLEX			= 0x00000008,
			DDS_HEADER_CAP_TEXTURE			= 0x00001000,
			DDS_HEADER_CAP_MIPMAP			= 0x00400000,
		};

		enum DDS_HEADER_CAPS2
		{
			DDS_HEADER_CAP2_CUBEMAP					= 0x00000200,
			DDS_HEADER_CAP2_CUBEMAP_POSITIVE_X		= 0x00000400,
			DDS_HEADER_CAP2_CUBEMAP_NEGATIVE_X		= 0x00000800,
			DDS_HEADER_CAP2_CUBEMAP_POSITIVE_Y		= 0x00001000,
			DDS_HEADER_CAP2_CUBEMAP_NEGATIVE_Y		= 0x00002000,
			DDS_HEADER_CAP2_CUBEMAP_POSITIVE_Z		= 0x00004000,
			DDS_HEADER_CAP2_CUBEMAP_NEGATIVE_Z		= 0x00008000,
			DDS_HEADER_CAP2_VOLUME					= 0x00200000
		};

		DDS_HEADER			m_header;
		SurfaceArray		m_surfaces;
	};
}
