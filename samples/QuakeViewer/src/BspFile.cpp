#include <assert.h>
#include <stdexcept>
#include "BspFile.h"

CBspFile::CBspFile(Framework::CStream& inputStream)
{
	uint8 signature[4];
	inputStream.Read(signature, 4);
	if(memcmp(signature, "IBSP", 4))
	{
		throw std::runtime_error("Invalid BSP. Bad signature.");
	}
	
	uint32 version = inputStream.Read32();
	assert(version == 0x2E);

	DIRENTRY dirEntries[DIRENTRY_COUNT];
	for(unsigned int i = 0; i < DIRENTRY_COUNT; i++)
	{
		dirEntries[i].offset = inputStream.Read32();
		dirEntries[i].length = inputStream.Read32();
	}

	ReadLumpData(inputStream, dirEntries[LUMP_ENTITIES], m_entities);
	ReadLumpData(inputStream, dirEntries[LUMP_PLANES], m_planes);
	ReadLumpData(inputStream, dirEntries[LUMP_NODES], m_nodes);
	ReadLumpData(inputStream, dirEntries[LUMP_LEAVES], m_leaves);
	ReadLumpData(inputStream, dirEntries[LUMP_LEAFFACES], m_leafFaces);
	ReadLumpData(inputStream, dirEntries[LUMP_TEXTURES], m_textures);
	ReadLumpData(inputStream, dirEntries[LUMP_VERTICES], m_vertices);
	ReadLumpData(inputStream, dirEntries[LUMP_MESHVERTICES], m_meshVertices);
	ReadLumpData(inputStream, dirEntries[LUMP_EFFECTS], m_effects);
	ReadLumpData(inputStream, dirEntries[LUMP_FACES], m_faces);
	ReadLumpData(inputStream, dirEntries[LUMP_LIGHTMAPS], m_lightMaps);

	//Read VISDATA
	{
		inputStream.Seek(dirEntries[LUMP_VISDATA].offset, Framework::STREAM_SEEK_SET);
		m_visData.vectorCount = inputStream.Read32();
		m_visData.vectorSize = inputStream.Read32();
		uint32 visDataSize = m_visData.vectorCount * m_visData.vectorSize;
		m_visData.vectors.resize(visDataSize);
		inputStream.Read(&m_visData.vectors[0], visDataSize);
	}
}

CBspFile::~CBspFile()
{

}

const char* CBspFile::GetEntities() const
{
	return static_cast<const char*>(&m_entities[0]);
}

const Bsp::TextureArray& CBspFile::GetTextures() const
{
	return m_textures;
}

const Bsp::PlaneArray& CBspFile::GetPlanes() const
{
	return m_planes;
}

const Bsp::NodeArray& CBspFile::GetNodes() const
{
	return m_nodes;
}

const Bsp::LeafArray& CBspFile::GetLeaves() const
{
	return m_leaves;
}

const Bsp::LeafFaceArray& CBspFile::GetLeafFaces() const
{
	return m_leafFaces;
}

const Bsp::VertexArray& CBspFile::GetVertices() const
{
	return m_vertices;
}

const Bsp::MeshVertexArray& CBspFile::GetMeshVertices() const
{
	return m_meshVertices;
}

const Bsp::FaceArray& CBspFile::GetFaces() const
{
	return m_faces;
}

const Bsp::LightMapArray& CBspFile::GetLightMaps() const
{
	return m_lightMaps;
}

const Bsp::VISDATA& CBspFile::GetVisData() const
{
	return m_visData;
}
