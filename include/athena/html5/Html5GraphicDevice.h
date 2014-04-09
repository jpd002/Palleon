#pragma once

#include <EGL/EGL.h>
#include "../GraphicDevice.h"

namespace Athena
{
	class CHtml5GraphicDevice : public CGraphicDevice
	{
	public:
		static void						CreateInstance();

		virtual void					Draw() override;
		
		virtual VertexBufferPtr			CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&) override;
		virtual TexturePtr				CreateTexture(TEXTURE_FORMAT, uint32, uint32, uint32) override;
		virtual TexturePtr				CreateCubeTexture(TEXTURE_FORMAT, uint32) override;
		virtual RenderTargetPtr			CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32) override;
		virtual CubeRenderTargetPtr		CreateCubeRenderTarget(TEXTURE_FORMAT, uint32) override;
		
	private:
										CHtml5GraphicDevice();
		virtual							~CHtml5GraphicDevice();
	};
}
