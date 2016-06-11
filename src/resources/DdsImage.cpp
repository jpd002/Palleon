#include "palleon/resources/DdsImage.h"
#include <assert.h>
#include <stdexcept>
#include <cstring>

using namespace Palleon;

CDdsImage::CDdsImage(Framework::CStream& inputStream)
{
	memset(&m_header, 0, sizeof(m_header));
	Read(inputStream);
}

CDdsImage::~CDdsImage()
{

}

uint32 CDdsImage::GetWidth() const
{
	return m_header.width;
}

uint32 CDdsImage::GetHeight() const
{
	return m_header.height;
}

uint32 CDdsImage::GetPitch() const
{
	return (m_header.ddspf.rgbBitCount / 8) * m_header.width;
}

bool CDdsImage::IsCubeMap() const
{
	return (m_header.caps2 & DDS_HEADER_CAP2_CUBEMAP) != 0;
}

const CDdsImage::SurfaceArray& CDdsImage::GetSurfaces() const
{
	return m_surfaces;
}

void CDdsImage::Read(Framework::CStream& inputStream)
{
	uint32 signature = inputStream.Read32();
	if(signature != ' SDD')
	{
		assert(0);
		throw std::runtime_error("Invalid DDS image (Invalid signature).");
	}

	inputStream.Read(&m_header, sizeof(DDS_HEADER));
	if(m_header.size != sizeof(DDS_HEADER))
	{
		assert(0);
		throw std::runtime_error("Invalid DDS image (Invalid header size).");
	}

	if(m_header.ddspf.size != sizeof(DDS_PIXELFORMAT))
	{
		assert(0);
		throw std::runtime_error("Invalid DDS image (Invalid header size).");
	}

	const uint32 mandatoryFlags = DDS_HEADER_FLAG_CAPS | DDS_HEADER_FLAG_HEIGHT | DDS_HEADER_FLAG_WIDTH | DDS_HEADER_FLAG_PIXELFORMAT;
	if((m_header.flags & mandatoryFlags) != mandatoryFlags)
	{
		assert(0);
		throw std::runtime_error("Invalid DDS image (Missing flags).");
	}

	assert((m_header.ddspf.flags & (DDS_PIXELFORMAT_FLAG_RGB | DDS_PIXELFORMAT_FLAG_ALPHAPIXELS)) != 0);

	uint32 surfaceSize = (m_header.ddspf.rgbBitCount / 8) * m_header.width * m_header.height;

	if(m_header.caps2 & DDS_HEADER_CAP2_CUBEMAP)
	{
		const uint32 fullCubeMapFlags = 
			DDS_HEADER_CAP2_CUBEMAP_POSITIVE_X | DDS_HEADER_CAP2_CUBEMAP_NEGATIVE_X |
			DDS_HEADER_CAP2_CUBEMAP_POSITIVE_Y | DDS_HEADER_CAP2_CUBEMAP_NEGATIVE_Y |
			DDS_HEADER_CAP2_CUBEMAP_POSITIVE_Z | DDS_HEADER_CAP2_CUBEMAP_NEGATIVE_Z;
		assert((m_header.caps2 & fullCubeMapFlags) == fullCubeMapFlags);

		for(unsigned int i = 0; i < 6; i++)
		{
			SurfaceByteArray surface(surfaceSize);
			inputStream.Read(surface.data(), surfaceSize);
			m_surfaces.push_back(std::move(surface));
		}
	}
}
