#pragma once

#include "../RenderTarget.h"
#include "GlEsGraphicDevice.h"

namespace Palleon
{
	class CGlEsRenderTarget : public CRenderTarget
	{
	public:
								CGlEsRenderTarget(TEXTURE_FORMAT, uint32, uint32);
		virtual					~CGlEsRenderTarget();

		void					Update(uint32, const void*) override;
		void					UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;
		void*					GetHandle() const override;

		void					Clear() override;
		void					Draw(const ViewportPtr&) override;

	private:
		uint32					m_width = 0;
		uint32					m_height = 0;
		GLuint					m_texture = 0;
		GLuint					m_depthRenderBuffer = 0;
		GLuint					m_frameBuffer = 0;
	};
}
