#pragma once

#include "Types.h"
#include "athena/Texture.h"
#include "OpenGlEsDefs.h"

namespace Athena
{
	class CIosTexture : public CTexture
	{
	public:
												CIosTexture(GLuint, bool = false);
		virtual									~CIosTexture();
		
		static TexturePtr						Create(TEXTURE_FORMAT, uint32, uint32);
		static TexturePtr						CreateFromFile(const char*);
		static TexturePtr						CreateFromMemory(const void*, uint32);
		
		static TexturePtr						CreateCubeFromFile(const char*);
				
		void									Update(const void*);
		
		void*									GetHandle() const override;
		bool									IsCubeMap() const;
		
	protected:
		static GLuint							LoadFromData(void*);
		static GLuint							TryLoadTGA(void*);

		static GLuint							LoadCubeFromPVR(void*);
		
		GLuint									m_texture;
		bool									m_isCubeMap;
	};
}
