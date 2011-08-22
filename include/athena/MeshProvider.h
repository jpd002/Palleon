#ifndef _MESHPROVIDER_H_
#define _MESHPROVIDER_H_

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

#endif
