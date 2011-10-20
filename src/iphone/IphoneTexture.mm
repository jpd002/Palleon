#include "IphoneTexture.h"
#include <assert.h>
#include <vector>
#import <UIKit/UIKit.h>

using namespace Athena;

uint32 GetNextPowerOfTwo(uint32 number)
{
    uint32 currentNumber = 1;
    while(currentNumber < number)
    {
        currentNumber <<= 1;
    }
    return currentNumber;
}

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
: m_texture(0)
{
    NSData* texData = [[NSData alloc] initWithBytesNoCopy: const_cast<void*>(data) length: size freeWhenDone: NO];
    LoadFromData(texData);
    [texData release];
}

CIphoneTexture::CIphoneTexture(const void* data, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
: m_texture(0)
{
    glGenTextures(1, &m_texture);
    assert(glGetError() == GL_NO_ERROR);
    
    assert(textureFormat == TEXTURE_FORMAT_RGB888);
    
    uint32 pixelCount = width * height;

    uint16* imageData = new uint16[pixelCount];
    memset(imageData, 0, pixelCount * sizeof(uint16));
    
    for(unsigned int i = 0; i < pixelCount; i++)
    {
        uint16 b = reinterpret_cast<const uint8*>(data)[(i * 3) + 0];
        uint16 g = reinterpret_cast<const uint8*>(data)[(i * 3) + 1];
        uint16 r = reinterpret_cast<const uint8*>(data)[(i * 3) + 2];
        uint16 dstPixel = (r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11);
        reinterpret_cast<uint16*>(imageData)[i] = dstPixel;
    }
    
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, imageData);
    assert(glGetError() == GL_NO_ERROR);
    
    delete [] imageData;
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
    if(image == nil)
    {
        TryLoadTGA(texDataPtr);
        return;
    }
    
    GLuint srcWidth = CGImageGetWidth(image.CGImage);
    GLuint srcHeight = CGImageGetHeight(image.CGImage);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

    GLuint dstWidth = GetNextPowerOfTwo(srcWidth);
    GLuint dstHeight = GetNextPowerOfTwo(srcHeight);

    void* imageData = malloc( dstHeight * dstWidth * 4 );
        
    CGContextRef context = CGBitmapContextCreate( imageData, dstWidth, dstHeight, 8, 4 * dstWidth, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
    CGColorSpaceRelease( colorSpace );
    CGContextClearRect( context, CGRectMake( 0, 0, dstWidth, dstHeight ) );
    CGContextTranslateCTM( context, 0, dstHeight - dstHeight );
    CGContextDrawImage( context, CGRectMake( 0, 0, dstWidth, dstHeight ), image.CGImage );
    
    for(unsigned int i = 0; i < dstWidth * dstHeight; i++)
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dstWidth, dstHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, imageData);
    assert(glGetError() == GL_NO_ERROR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    CGContextRelease(context);
    
    free(imageData);
    [image release];
}

bool CIphoneTexture::TryLoadTGA(void* texDataPtr)
{
#pragma pack(push, 1)
    struct TGAHEADER
    {
        uint8   idSize;
        uint8   colorMapType;
        uint8   imageType;
        
        int16   colorMapStart;
        int16   colorMapLength;
        uint8   colorMapBits;
        
        int16   startX;
        int16   startY;
        int16   width;
        int16   height;
        
        uint8   bits;
        uint8   descriptor;
    };
#pragma pack(pop)
    
    NSData* texData = reinterpret_cast<NSData*>(texDataPtr);
    const uint8* tgaFile = reinterpret_cast<const uint8*>([texData bytes]);
    uint32 tgaFileSize = [texData length];
    
    if(tgaFileSize < sizeof(TGAHEADER))
    {
        assert(0);
        return false;
    }
    
    const TGAHEADER* header = reinterpret_cast<const TGAHEADER*>(tgaFile);
    if(header->imageType != 0x02)
    {
        assert(0);
        return false;
    }
    
    if(header->bits != 32)
    {
        assert(0);
        return false;
    }

    const uint32* srcImage = reinterpret_cast<const uint32*>(tgaFile + sizeof(TGAHEADER));
    uint16* dstImage = new uint16[header->height * header->width];
    for(unsigned int y = 0; y < header->height; y++)
    {
        unsigned int srcY = header->height - y - 1;
        for(unsigned int x = 0; x < header->width; x++)
        {
            uint32 srcPixel = srcImage[x + (srcY * header->width)];
            uint16 b = static_cast<uint8>(srcPixel >>  0);
            uint16 g = static_cast<uint8>(srcPixel >>  8);
            uint16 r = static_cast<uint8>(srcPixel >> 16);
            uint16 a = static_cast<uint8>(srcPixel >> 24);
            uint16 dstPixel = (a >> 4) | ((b >> 4) << 4) | ((g >> 4) << 8) | ((r >> 4) << 12);
            dstImage[x + (y * header->width)] = dstPixel;
        }
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header->width, header->height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, dstImage);
    assert(glGetError() == GL_NO_ERROR);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    delete dstImage;
    
    return true;
}
