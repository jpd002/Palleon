#pragma once

#include "palleon/graphics/Mesh.h"

namespace Palleon
{
	class CFullscreenQuad : public CMesh
	{
	public:
							CFullscreenQuad();
		virtual				~CFullscreenQuad();

		static MeshPtr		Create();

	private:
		void				UpdateVertices();
		void				UpdateIndices();
	};
}
