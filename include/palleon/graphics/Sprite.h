#pragma once

#include "palleon/graphics/Mesh.h"
#include "palleon/ILayoutable.h"

namespace Palleon
{
	class CSprite;
	typedef std::shared_ptr<CSprite> SpritePtr;

	class CSprite : public CMesh, public ILayoutable
	{
	public:
							CSprite();
		virtual				~CSprite();

		static SpritePtr	Create();

		void				Update(float dt) override;
		
		void				SetPosition(const CVector3&) override;

		CVector2			GetSize() const;
		void				SetSize(const CVector2&) override;
		
	protected:
		void				UpdateVertices();
		void				UpdateIndices();

		enum STATUS_FLAGS
		{
			STATUS_VERTEXBUFFER_DIRTY = 0x01,
		};

		CVector2			m_size;
		uint32				m_status;
	};
}
