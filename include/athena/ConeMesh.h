#pragma once

#include "Mesh.h"

namespace Athena
{
	class CConeMesh : public CMesh
	{
	public:
							CConeMesh();
		virtual				~CConeMesh();

		static MeshPtr		Create();
	};
}
