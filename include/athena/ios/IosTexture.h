#pragma once

#include <string>
#include "Types.h"
#include "athena/Texture.h"
#include "OpenGlEsDefs.h"

namespace Athena
{
	class CIosTexture : public CTexture
	{
	public:
												CIosTexture(GLuint);
		virtual									~CIosTexture();
		
		static TexturePtr						Create(TEXTURE_FORMAT, uint32, uint32);
		static TexturePtr						CreateCube(TEXTURE_FORMAT, uint32);

		void									Update(uint32, const void*) override;
		void									UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;
		
		void*									GetHandle() const override;
		
	protected:
		void									UpdateTarget(GLenum, GLenum, const void*);
		
		GLuint									m_texture;
	};
}
