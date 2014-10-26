#pragma once

#include <list>
#include <string>
#include "Types.h"
#include "palleon/graphics/Viewport.h"
#include "palleon/graphics/VertexBuffer.h"
#include "palleon/graphics/Texture.h"
#include "palleon/graphics/RenderTarget.h"
#include "palleon/graphics/CubeRenderTarget.h"
#include "palleon/graphics/Color.h"
#include "palleon/graphics/EffectProvider.h"

namespace Palleon
{
	class CGraphicDevice
	{
	public:
		virtual								~CGraphicDevice();

		static CGraphicDevice&				GetInstance();

		virtual void						Draw() = 0;

		CVector2							GetScreenSize() const;

		uint32								GetDrawCallCount() const;
		float								GetFrameRate() const;

		virtual VertexBufferPtr				CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&) = 0;

		virtual TexturePtr					CreateTexture(TEXTURE_FORMAT format, uint32 width, uint32 height, uint32 mipCount) = 0;
		virtual TexturePtr					CreateCubeTexture(TEXTURE_FORMAT, uint32) = 0;

		virtual RenderTargetPtr				CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32) = 0;
		virtual CubeRenderTargetPtr			CreateCubeRenderTarget(TEXTURE_FORMAT, uint32) = 0;
		
		EffectProviderPtr					GetDefaultEffectProvider() const;

		void								AddViewport(CViewport*);
		void								AddViewportAfter(CViewport*, CViewport*);
		void								RemoveViewport(CViewport*);

		static uint32						ConvertColorToUInt32(const CColor&);

	protected:
		typedef std::list<CViewport*>		ViewportList;

											CGraphicDevice();

		static CGraphicDevice*				m_instance;

		ViewportList						m_viewports;
		CVector2							m_screenSize;

		uint32								m_drawCallCount;
		float								m_frameRate;

		EffectProviderPtr					m_defaultEffectProvider;
	};
}
