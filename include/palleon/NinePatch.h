#pragma once

#include "palleon/Mesh.h"
#include "palleon/resources/NinePatchDescriptor.h"

namespace Palleon
{
	class CNinePatch;
	typedef std::shared_ptr<CNinePatch> NinePatchPtr;

	class CNinePatch : public CMesh
	{
	public:
									CNinePatch();
		virtual						~CNinePatch();

		static NinePatchPtr			Create();

		virtual void				Update(float dt) override;

		void						SetSize(const CVector2&);

		void						SetDescriptor(const CNinePatchDescriptor*);

	protected:
		void						UpdateVertices();
		void						UpdateIndices();

		enum STATUS_FLAGS
		{
			STATUS_VERTEXBUFFER_DIRTY = 0x01,
		};

		CVector2					m_size = CVector2(1, 1);
		uint32						m_status = 0;
		const CNinePatchDescriptor*	m_descriptor = nullptr;
	};
};
