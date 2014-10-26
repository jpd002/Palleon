#pragma once

#include "Mesh.h"

namespace Palleon
{
	class CConeMesh : public CMesh
	{
	public:
							CConeMesh();
		virtual				~CConeMesh();

		static MeshPtr		Create();
	};
}
