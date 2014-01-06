#ifndef _BSPFILE_H_
#define _BSPFILE_H_

#include "BspDefs.h"
#include "Stream.h"

class CBspFile
{
public:
									CBspFile(Framework::CStream&);
	virtual							~CBspFile();

	const char*						GetEntities() const;
	const Bsp::TextureArray&		GetTextures() const;
	const Bsp::PlaneArray&			GetPlanes() const;
	const Bsp::NodeArray&			GetNodes() const;
	const Bsp::LeafArray&			GetLeaves() const;
	const Bsp::LeafFaceArray&		GetLeafFaces() const;
	const Bsp::VertexArray&			GetVertices() const;
	const Bsp::MeshVertexArray&		GetMeshVertices() const;
	const Bsp::FaceArray&			GetFaces() const;
	const Bsp::LightMapArray&		GetLightMaps() const;
	const Bsp::VISDATA&				GetVisData() const;

private:
	struct DIRENTRY
	{
		uint32 offset;
		uint32 length;
	};

	enum
	{
		DIRENTRY_COUNT		= 17,
	};

	enum
	{
		LUMP_ENTITIES		= 0,
		LUMP_TEXTURES		= 1,
		LUMP_PLANES			= 2,
		LUMP_NODES			= 3,
		LUMP_LEAVES			= 4,
		LUMP_LEAFFACES		= 5,
		LUMP_VERTICES		= 10,
		LUMP_MESHVERTICES	= 11,
		LUMP_EFFECTS		= 12,
		LUMP_FACES			= 13,
		LUMP_LIGHTMAPS		= 14,
		LUMP_VISDATA		= 16,
	};

	template <typename DataType>
	void ReadLumpData(Framework::CStream& stream, const DIRENTRY& dirEntry, std::vector<DataType>& dataArray)
	{
		assert(dirEntry.length % sizeof(DataType) == 0);
		uint32 entryCount = dirEntry.length / sizeof(DataType);
		if(entryCount == 0) return;

		stream.Seek(dirEntry.offset, Framework::STREAM_SEEK_SET);

		dataArray.resize(entryCount);
		stream.Read(&dataArray[0], dirEntry.length);
	}

	Bsp::EntityArray		m_entities;
	Bsp::TextureArray		m_textures;
	Bsp::PlaneArray			m_planes;
	Bsp::NodeArray			m_nodes;
	Bsp::LeafArray			m_leaves;
	Bsp::LeafFaceArray		m_leafFaces;
	Bsp::VertexArray		m_vertices;
	Bsp::MeshVertexArray	m_meshVertices;
	Bsp::EffectArray		m_effects;
	Bsp::FaceArray			m_faces;
	Bsp::LightMapArray		m_lightMaps;
	Bsp::VISDATA			m_visData;
};

#endif
