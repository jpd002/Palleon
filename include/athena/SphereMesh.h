#pragma once

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
