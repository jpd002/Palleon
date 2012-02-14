#ifndef _CUBEMESH_H_
#define _CUBEMESH_H_

#include "Mesh.h"

namespace Athena
{
	class CCubeMesh : public CMesh
	{
	public:
							CCubeMesh();
		virtual				~CCubeMesh();

		static MeshPtr		Create();
	};
}

#endif
