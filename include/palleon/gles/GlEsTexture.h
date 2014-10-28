#pragma once

#include <string>
#include "Types.h"
#include "palleon/graphics/Texture.h"
#include "OpenGlEsDefs.h"

namespace Palleon
{
	class CGlEsTexture : public CTexture
	{
	public:
												CGlEsTexture(GLuint);
		virtual									~CGlEsTexture();
		
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
