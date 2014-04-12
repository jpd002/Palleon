#pragma once

#include "Mesh.h"
#include "Camera.h"

namespace Athena
{
	typedef std::vector<CMesh*> MeshArray;

	class CMeshProvider : public CSceneNode
	{
	public:
		virtual			~CMeshProvider() {}

		virtual	void	GetMeshes(MeshArray&, const CCamera*) = 0;

	private:

	};
}
