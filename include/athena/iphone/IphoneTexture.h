#ifndef _IPHONETEXTURE_H_
#define _IPHONETEXTURE_H_

#include "Types.h"
#include "athena/Texture.h"
#include "OpenGlEsDefs.h"

namespace Athena
{
	class CIphoneTexture : public CTexture
	{
	public:
												CIphoneTexture(GLuint, bool = false);
		virtual									~CIphoneTexture();
		
		static TexturePtr						Create(TEXTURE_FORMAT, uint32, uint32);
		static TexturePtr						CreateFromFile(const char*);
		static TexturePtr						CreateFromMemory(const void*, uint32);
		
		static TexturePtr						CreateCubeFromFile(const char*);
				
		void									Update(const void*);
		
		void*									GetHandle() const;
		bool									IsCubeMap() const;
		
	protected:
		static GLuint							LoadFromData(void*);
		static GLuint							TryLoadTGA(void*);

		static GLuint							LoadCubeFromPVR(void*);
		
		GLuint									m_texture;
		bool									m_isCubeMap;
	};
}

#endif
