#ifndef _ATHENA_NINEPATCH_H_
#define _ATHENA_NINEPATCH_H_

#include "athena/Mesh.h"
#include "athena/NinePatchDescriptor.h"

namespace Athena
{
	class CNinePatch;
	typedef std::tr1::shared_ptr<CNinePatch> NinePatchPtr;

	class CNinePatch : public CMesh
	{
	public:
		virtual						~CNinePatch();

		static NinePatchPtr			Create();

		virtual void				Update(float dt);

		void						SetSize(const CVector2&);

		void						SetDescriptor(const CNinePatchDescriptor*);

	protected:
									CNinePatch();
					
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

#endif
