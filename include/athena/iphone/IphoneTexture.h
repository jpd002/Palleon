#ifndef _IPHONETEXTURE_H_
#define _IPHONETEXTURE_H_

#include "Types.h"
#include "athena/Texture.h"
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

namespace Athena
{
    class CIphoneTexture : public CTexture
    {
    public:
                                                CIphoneTexture(const char*);
                                                CIphoneTexture(const void*, uint32);
                                                CIphoneTexture(const void*, TEXTURE_FORMAT, uint32, uint32);
		virtual									~CIphoneTexture();
        
        GLuint                                  GetTexture() const;
        
    protected:
        void                                    LoadFromData(void*);
        bool                                    TryLoadTGA(void*);
        
        GLuint                                  m_texture;
    };
}

#endif
