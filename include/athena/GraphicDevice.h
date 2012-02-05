#ifndef _GRAPHICDEVICE_H_
#define _GRAPHICDEVICE_H_

#include <list>
#include "Types.h"
#include "Viewport.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Color.h"

namespace Athena
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

		virtual TexturePtr					CreateTextureFromFile(const char*) = 0;
		virtual TexturePtr					CreateTextureFromMemory(const void*, uint32) = 0;
		virtual TexturePtr					CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32) = 0;

		virtual TexturePtr					CreateCubeTextureFromFile(const char*) = 0;

		void								AddViewport(CViewport*);
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
	};
}

#endif
