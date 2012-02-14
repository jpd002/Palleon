#ifndef _SPHEREMESH_H_
#define _SPHEREMESH_H_

#include "Mesh.h"

namespace Athena
{
	class CSphereMesh : public CMesh
	{
	public:
							CSphereMesh();
		virtual				~CSphereMesh();

		static MeshPtr		Create();
	};
}

#endif
