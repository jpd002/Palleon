#ifndef _BSPMAPMESHPROVIDER_H_
#define _BSPMAPMESHPROVIDER_H_

#include "PalleonEngine.h"
#include "BspFile.h"
#include "BspMapResourceProvider.h"
#include "BatchMesh.h"
#include "BspFaceMesh.h"

class CBspMapMeshProvider : public Palleon::CMeshProvider
{
public:
								CBspMapMeshProvider(CBspFile*, CBspMapResourceProvider*);
	virtual						~CBspMapMeshProvider();

	void						GetMeshes(Palleon::MeshArray&, const Palleon::CCamera*) override;

private:
	typedef std::vector<BspFaceMeshPtr> FaceMeshArray;
	typedef std::vector<BatchMeshPtr> BatchArray;
	typedef std::vector<uint32> FaceIndexArray;

	void						LoadFaces();
	unsigned int				FindLeaf(const CVector3&) const;

	CBspFile*					m_bspFile;
	CBspMapResourceProvider*	m_bspMapResourceProvider;
	FaceMeshArray				m_faceMeshes;
	BatchArray					m_batches;
	uint8*						m_faceVisibleSet;
	uint32						m_faceVisibleSetSize;
	FaceIndexArray				m_visibleFaceIndices;
};

typedef std::shared_ptr<CBspMapMeshProvider> BspMapMeshProviderPtr;

#endif
