#include "palleon/ios/MetalTexture.h"

using namespace Palleon;

CMetalTexture::CMetalTexture(id<MTLTexture> texture)
: m_texture(texture)
{
	
}

CMetalTexture::~CMetalTexture()
{
	[m_texture release];
}

TexturePtr CMetalTexture::Create(id<MTLDevice> device, TEXTURE_FORMAT format, uint32 width, uint32 height)
{
	MTLTextureDescriptor* textureDescriptor = [[MTLTextureDescriptor alloc] init];
	textureDescriptor.pixelFormat = MTLPixelFormatRGBA8Unorm;
	textureDescriptor.textureType = MTLTextureType2D;
	textureDescriptor.width = width;
	textureDescriptor.height = height;
	textureDescriptor.mipmapLevelCount = 1;

	id<MTLTexture> texture = [device newTextureWithDescriptor: textureDescriptor];
	[textureDescriptor release];

	auto result = std::make_shared<CMetalTexture>(texture);
	result->m_format = format;
	result->m_width = width;
	result->m_height = height;
	result->m_mipCount = 1;
	return result;
}

void CMetalTexture::Update(uint32, const void* data)
{
	uint32 srcPitch = (c_textureFormatSize[m_format] * m_width) / 8;
	[m_texture copyFromPixels: data rowBytes: srcPitch imageBytes: 0 toSlice: 0 mipmapLevel: 0 origin: MTLOriginMake(0, 0, 0) size: MTLSizeMake(m_width, m_height, 1)];
}

void CMetalTexture::UpdateCubeFace(TEXTURE_CUBE_FACE, const void*)
{
	
}

void* CMetalTexture::GetHandle() const
{
	return m_texture;
}
