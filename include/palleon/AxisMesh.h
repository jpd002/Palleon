#pragma once

#include "athena/Mesh.h"

namespace Athena
{
	class CAxisMesh : public Athena::CMesh
	{
	public:
							CAxisMesh();
		virtual				~CAxisMesh();

		static MeshPtr		Create();
	};
}
