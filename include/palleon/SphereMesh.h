#pragma once

#include "Mesh.h"

namespace Palleon
{
	class CSphereMesh : public CMesh
	{
	public:
							CSphereMesh();
		virtual				~CSphereMesh();

		static MeshPtr		Create();
	};
}
