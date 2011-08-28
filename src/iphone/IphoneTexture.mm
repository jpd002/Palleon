#include "IphoneTexture.h"
#include <assert.h>
#include <vector>
#import <UIKit/UIKit.h>

using namespace Athena;

CIphoneTexture::CIphoneTexture(const char* path)
: m_texture(0)
{
    NSString* pathString = [NSString stringWithUTF8String: path];
    NSData* texData = [[NSData alloc] initWithContentsOfFile: pathString];
    assert(texData != NULL);
    LoadFromData(texData);
    [texData release];
}

CIphoneTexture::CIphoneTexture(const void* data, uint32 size)
{
    NSData* texData = [[NSData alloc] initWithBytesNoCopy: const_cast<void*>(data) length: size freeWhenDone: NO];
    LoadFromData(texData);
    [texData release];
}

CIphoneTexture::~CIphoneTexture()
{
    glDeleteTextures(1, &m_texture);
}

GLuint CIphoneTexture::GetTexture() const
{
    return m_texture;
}

void CIphoneTexture::LoadFromData(void* texDataPtr)
{
    NSData* texData = reinterpret_cast<NSData*>(texDataPtr);
    
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
    
    for(unsigned int i = 0; i < height * width; i++)
    {
        uint32 srcPixel = reinterpret_cast<uint32*>(imageData)[i];
        uint16 a = static_cast<uint8>(srcPixel >>  0);
        uint16 b = static_cast<uint8>(srcPixel >>  8);
        uint16 g = static_cast<uint8>(srcPixel >> 16);
        uint16 r = static_cast<uint8>(srcPixel >> 24);
        uint16 dstPixel = (r >> 4) | ((g >> 4) << 4) | ((b >> 4) << 8) | ((a >> 4) << 12);
        reinterpret_cast<uint16*>(imageData)[i] = dstPixel;
    }
    
    glGenTextures(1, &m_texture);
    assert(glGetError() == GL_NO_ERROR);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, imageData);
    assert(glGetError() == GL_NO_ERROR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    CGContextRelease(context);
    
    free(imageData);
    [image release];
}
