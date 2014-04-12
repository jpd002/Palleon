#pragma once

#include "athena/Mesh.h"
#include "athena/resources/NinePatchDescriptor.h"

namespace Athena
{
	class CNinePatch;
	typedef std::shared_ptr<CNinePatch> NinePatchPtr;

	class CNinePatch : public CMesh
	{
	public:
									CNinePatch();
		virtual						~CNinePatch();

		static NinePatchPtr			Create();

		virtual void				Update(float dt);

		void						SetSize(const CVector2&);

		void						SetDescriptor(const CNinePatchDescriptor*);

	protected:
		void						UpdateVertices();
		void						UpdateIndices();

		enum STATUS_FLAGS
		{
			STATUS_VERTEXBUFFER_DIRTY = 0x01,
		};

		CVector2					m_size;
		uint32						m_status;
		const CNinePatchDescriptor*	m_descriptor;
	};
};
