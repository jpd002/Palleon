#include "athena/Texture.h"

using namespace Athena;

const uint32 Athena::c_textureFormatSize[TEXTURE_FORMAT_MAX] =
{
	0,
	24,
	32,
	4
};

CTexture::CTexture()
: m_format(TEXTURE_FORMAT_UNKNOWN)
, m_width(0)
, m_height(0)
{

}

CTexture::~CTexture()
{

}
