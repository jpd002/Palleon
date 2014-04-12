#pragma once

#include "palleon/Mesh.h"

namespace Palleon
{
	class CAxisMesh : public Palleon::CMesh
	{
	public:
							CAxisMesh();
		virtual				~CAxisMesh();

		static MeshPtr		Create();
	};
}
