#pragma once

#include "palleon/graphics/Mesh.h"

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
