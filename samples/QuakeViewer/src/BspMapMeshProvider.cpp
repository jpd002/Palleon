#include "BspMapMeshProvider.h"
#include "BspUtils.h"
#include <assert.h>

class FaceSorter
{
public:
	FaceSorter(const Bsp::FaceArray& faces)
		: m_faces(faces)
	{

	}

	bool operator()(uint32 faceIdx1, uint32 faceIdx2) const
	{
		const Bsp::FACE& face1 = m_faces[faceIdx1];
		const Bsp::FACE& face2 = m_faces[faceIdx2];
		if(face1.textureIndex == face2.textureIndex)
		{
			return face1.lightMapIndex < face2.lightMapIndex;
		}
		else
		{
			return face1.textureIndex < face2.textureIndex;
		}
	}

private:
	const Bsp::FaceArray&	m_faces;
};

CBspMapMeshProvider::CBspMapMeshProvider(CBspFile* bspFile, CBspMapResourceProvider* bspMapResourceProvider)
: m_bspFile(bspFile)
, m_bspMapResourceProvider(bspMapResourceProvider)
, m_faceVisibleSet(NULL)
, m_faceVisibleSetSize(0)
{
	LoadFaces();

	const unsigned int batchCount = 256;
	m_batches.resize(batchCount);
	for(unsigned int i = 0; i < batchCount; i++)
	{
		m_batches[i] = BatchMeshPtr(new CBatchMesh(1024, 4096, 
			Palleon::VERTEX_BUFFER_HAS_POS | Palleon::VERTEX_BUFFER_HAS_UV0 | Palleon::VERTEX_BUFFER_HAS_UV1 | Palleon::VERTEX_BUFFER_HAS_COLOR));
	}
}

CBspMapMeshProvider::~CBspMapMeshProvider()
{

}

void CBspMapMeshProvider::GetMeshes(Palleon::MeshArray& meshes, const Palleon::CCamera* camera)
{
	const Bsp::LeafArray& leaves(m_bspFile->GetLeaves());
	const Bsp::LeafFaceArray& leafFaces(m_bspFile->GetLeafFaces());
	const Bsp::FaceArray& faces(m_bspFile->GetFaces());
	const Bsp::VISDATA& visData(m_bspFile->GetVisData());

	CMatrix4 invViewMatrix = camera->GetViewMatrix().Inverse();
	CVector3 cameraPosition = CVector3(invViewMatrix(3, 0), invViewMatrix(3, 1), invViewMatrix(3, 2));

	int32 leafIndex = FindLeaf(cameraPosition);
	const Bsp::LEAF& leaf(leaves[leafIndex]);

	CFrustum cameraFrustum = camera->GetFrustum();

	memset(m_faceVisibleSet, 0, m_faceVisibleSetSize);

	if(leaf.cluster != -1)
	{
		const uint8* visVector = &visData.vectors[leaf.cluster * visData.vectorSize];
		for(uint32 i = 0; i < leaves.size(); i++)
		{
			const Bsp::LEAF& testLeaf = leaves[i];
			uint32 testCluster = testLeaf.cluster;
			if(static_cast<int32>(testCluster) < 0) continue;
			uint8 visByte = visVector[testCluster / 8];
			bool visible = (visByte & (1 << (testCluster & 7))) != 0;
			if(visible)
			{
				CVector3 leafBoxMin = Bsp::ConvertToAthenaCoord(static_cast<float>(testLeaf.mins[0]), static_cast<float>(testLeaf.mins[1]), static_cast<float>(testLeaf.mins[2]));
				CVector3 leafBoxMax = Bsp::ConvertToAthenaCoord(static_cast<float>(testLeaf.maxs[0]), static_cast<float>(testLeaf.maxs[1]), static_cast<float>(testLeaf.maxs[2]));

				if(cameraFrustum.Intersects(leafBoxMin, leafBoxMax))
				{
					for(unsigned int i = 0; i < testLeaf.leafFaceCount; i++)
					{
						unsigned int faceIndex = leafFaces[i + testLeaf.leafFaceIndex];
						unsigned int faceByte = faceIndex / 8;
						assert(faceByte < m_faceVisibleSetSize);
						m_faceVisibleSet[faceByte] |= (1 << (faceIndex & 7));
					}
				}
			}
		}
	}

	m_visibleFaceIndices.clear();

	for(unsigned int i = 0; i < faces.size(); i++)
	{
		const Bsp::FACE& currentFace = faces[i];
		if(currentFace.textureIndex != -1)
		{
			BspMapMaterialPtr faceMaterial = m_bspMapResourceProvider->GetMaterial(currentFace.textureIndex);
			if(faceMaterial->GetIsSky()) continue;
		}

		unsigned int faceByte = i / 8;
		if(m_faceVisibleSet[faceByte] & (1 << (i & 7)))
		{
			m_visibleFaceIndices.push_back(i);
		}
	}

	if(m_visibleFaceIndices.size() == 0) return;

	std::sort(m_visibleFaceIndices.begin(), m_visibleFaceIndices.end(), FaceSorter(faces));

	uint32 currentTextureIdx = -2;
	uint32 currentLightMapIdx = -2;
	BatchMeshPtr currentBatch;
	unsigned int currentBatchIndex = 0;
	for(unsigned int i = 0; i < m_visibleFaceIndices.size(); i++)
	{
		uint32 faceIndex = m_visibleFaceIndices[i];
		const Bsp::FACE& currentFace = faces[faceIndex];
		BspFaceMeshPtr faceMesh = m_faceMeshes[faceIndex];
		if(!faceMesh) continue;
		bool needNewBatch = false;
		needNewBatch |= (currentFace.textureIndex != currentTextureIdx);
		needNewBatch |= (currentFace.lightMapIndex != currentLightMapIdx);
		needNewBatch |= (currentBatch && !currentBatch->CanWriteMesh(faceMesh->GetVertexCount(), faceMesh->GetIndexCount()));
		if(needNewBatch)
		{
			//Switch to new batch
			if(currentBatch)
			{
				currentBatch->EndBatch();
			}
			currentBatch = m_batches[currentBatchIndex++];
			meshes.push_back(currentBatch.get());
			currentBatch->BeginBatch();
			currentTextureIdx = currentFace.textureIndex;
			currentLightMapIdx = currentFace.lightMapIndex;

			//Prepare material
			Palleon::MaterialPtr batchMaterial = currentBatch->GetMaterial();
			BspMapMaterialPtr faceMaterial = m_bspMapResourceProvider->GetMaterial(currentTextureIdx);
			
			for(unsigned int i = 0; i < Palleon::CMaterial::MAX_TEXTURE_SLOTS; i++)
			{
				batchMaterial->SetTexture(i, Palleon::TexturePtr());
			}

			for(unsigned int i = 0; i < faceMaterial->GetPassCount(); i++)
			{
				const BspMapPassPtr& pass(faceMaterial->GetPass(i));
				Palleon::TEXTURE_COMBINE_MODE combineMode = pass->GetBlendingFunction();
				if(pass->GetTextureSource() == CBspMapPass::TEXTURE_SOURCE_DIFFUSE)
				{
					batchMaterial->SetTexture(i, pass->GetTexture());
					batchMaterial->SetTextureMatrix(i, pass->GetUvMatrix());
					batchMaterial->SetTextureCoordSource(i, Palleon::TEXTURE_COORD_UV0);
					batchMaterial->SetTextureCombineMode(i, combineMode);
				}
				else
				{
					batchMaterial->SetTexture(i, m_bspMapResourceProvider->GetLightMap(currentLightMapIdx));
					batchMaterial->SetTextureMatrix(i, CMatrix4::MakeIdentity());
					batchMaterial->SetTextureCoordSource(i, Palleon::TEXTURE_COORD_UV1);
					batchMaterial->SetTextureCombineMode(i, combineMode);
				}
				batchMaterial->SetTextureAddressModeU(i, Palleon::TEXTURE_ADDRESS_REPEAT);
				batchMaterial->SetTextureAddressModeV(i, Palleon::TEXTURE_ADDRESS_REPEAT);
			}
		}

		assert(currentBatch->CanWriteMesh(faceMesh->GetVertexCount(), faceMesh->GetIndexCount()));
		currentBatch->WriteMesh(faceMesh->GetVertices(), faceMesh->GetVertexCount(), faceMesh->GetIndices(), faceMesh->GetIndexCount());
	}

	if(currentBatch)
	{
		currentBatch->EndBatch();
	}
}

unsigned int CBspMapMeshProvider::FindLeaf(const CVector3& searchPosition) const
{
	int32 index = 0;
	const Bsp::NodeArray& nodes(m_bspFile->GetNodes());
	const Bsp::PlaneArray& planes(m_bspFile->GetPlanes());

	while(index >= 0)
	{
		const Bsp::NODE& node = nodes[index];
		const Bsp::PLANE& plane = planes[node.plane];

		CVector3 planeNormal = Bsp::ConvertToAthenaCoord(plane.normal);
		float distance = planeNormal.Dot(searchPosition) - plane.distance;

		if(distance >= 0)
		{
			index = node.children[0];
		}
		else
		{
			index = node.children[1];
		}
	}
	return -index - 1;
}

void CBspMapMeshProvider::LoadFaces()
{
	const Bsp::FaceArray& faces(m_bspFile->GetFaces());
	m_faceMeshes.resize(faces.size());
	m_visibleFaceIndices.reserve(faces.size());
	for(unsigned int i = 0; i < faces.size(); i++)
	{
		auto result = BspFaceMeshPtr(new CBspFaceMesh(*m_bspFile, i));
		m_faceMeshes[i] = result;
	}

	m_faceVisibleSetSize = (faces.size() + 7) / 8;
	m_faceVisibleSet = new uint8[m_faceVisibleSetSize];
}
