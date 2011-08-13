#include "ResourceManager.h"
#include <zlib.h>

static unsigned int MakeCrc(const char* inputString)
{
	uLong crc = 0;
	return crc32(crc, reinterpret_cast<const Bytef*>(inputString), strlen(inputString));
}

CResourceManager::CResourceManager()
{

}

CResourceManager::~CResourceManager()
{

}

CResourceManager& CResourceManager::GetInstance()
{
	static CResourceManager resourceManager;
	return resourceManager;
}

void CResourceManager::ReleaseAllResources()
{
	for(TextureMap::const_iterator textureIterator(m_textures.begin());
		textureIterator != m_textures.end(); textureIterator++)
	{
		GLuint texId = textureIterator->second;
		glDeleteTextures(1, &texId);
		assert(glGetError() == GL_NO_ERROR);
	}
	m_textures.clear();
	
	for(FontDescriptorMap::const_iterator fontDescriptorIterator(m_fontDescriptors.begin());
		fontDescriptorIterator != m_fontDescriptors.end(); fontDescriptorIterator++)
	{
		const CFontDescriptor* descriptor(fontDescriptorIterator->second);
		delete descriptor;
	}
	m_fontDescriptors.clear();
}

GLuint CResourceManager::GetTexture(const char* fileName) const
{
	unsigned int resId = MakeCrc(fileName);
	TextureMap::const_iterator textureIterator(m_textures.find(resId));
	assert(textureIterator != m_textures.end());
	if(textureIterator == m_textures.end())
	{
		return -1;
	}
	return textureIterator->second;
}

const CFontDescriptor* CResourceManager::GetFontDescriptor(const char* fileName) const
{
	unsigned int resId = MakeCrc(fileName);
	FontDescriptorMap::const_iterator fontDescriptorIterator(m_fontDescriptors.find(resId));
	assert(fontDescriptorIterator != m_fontDescriptors.end());
	if(fontDescriptorIterator == m_fontDescriptors.end())
	{
		return NULL;
	}
	return fontDescriptorIterator->second;	
}

void CResourceManager::LoadTexture(const char* fileName)
{
	GLuint result = -1;
	glGenTextures(1, &result);
	
	NSString* fileNameString = [NSString stringWithUTF8String: fileName];
    NSString* path = [[NSBundle mainBundle] pathForResource: fileNameString ofType: @"png" inDirectory: @"Images"];
    NSData* texData = [[NSData alloc] initWithContentsOfFile: path];
	assert(texData != NULL);
    UIImage* image = [[UIImage alloc] initWithData: texData];
	
    GLuint width = CGImageGetWidth(image.CGImage);
    GLuint height = CGImageGetHeight(image.CGImage);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    void* imageData = malloc( height * width * 4 );
	
    CGContextRef context = CGBitmapContextCreate( imageData, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
    CGColorSpaceRelease( colorSpace );
    CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
    CGContextTranslateCTM( context, 0, height - height );
    CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), image.CGImage );
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, result);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	
    assert(glGetError() == GL_NO_ERROR);
    
    CGContextRelease(context);
	
    free(imageData);
    [image release];
    [texData release];
	
	{
		unsigned int resId = MakeCrc(fileName);
		assert(m_textures.find(resId) == m_textures.end());
		m_textures[resId] = result;
	}
}

void CResourceManager::LoadFontDescriptor(const char* fileName)
{
	NSString* fileNameString = [NSString stringWithUTF8String: fileName];
    NSString* path = [[NSBundle mainBundle] pathForResource: fileNameString ofType: @"fnt" inDirectory: @"Images"];

	CFontDescriptor* result = new CFontDescriptor();
	result->Load([path UTF8String]);
	
	{
		unsigned int resId = MakeCrc(fileName);
		assert(m_fontDescriptors.find(resId) == m_fontDescriptors.end());
		m_fontDescriptors[resId] = result;
	}	
}
