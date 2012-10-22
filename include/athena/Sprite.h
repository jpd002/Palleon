#pragma once

#include "Mesh.h"

namespace Athena
{
	class CSprite;
	typedef std::shared_ptr<CSprite> SpritePtr;

	class CSprite : public CMesh
	{
	public:
							CSprite();
		virtual				~CSprite();

		static SpritePtr	Create();

		virtual void		Update(float dt);
		
		void				SetSize(const CVector2&);
		
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
