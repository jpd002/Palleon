#pragma once

#include "Mesh.h"

namespace Palleon
{
	class CCubeMesh : public CMesh
	{
	public:
							CCubeMesh();
		virtual				~CCubeMesh();

		static MeshPtr		Create();
	};
}
