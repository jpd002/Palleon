#pragma once

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
