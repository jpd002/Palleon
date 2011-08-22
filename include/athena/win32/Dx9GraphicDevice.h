#ifndef _DX9GRAPHICDEVICE_H_
#define _DX9GRAPHICDEVICE_H_

#include <d3d9.h>
#include <unordered_map>
#include "../GraphicDevice.h"
#include "../Mesh.h"

namespace Athena
{
	class CDx9GraphicDevice : public CGraphicDevice
	{
	public:
		static void						CreateInstance(IDirect3DDevice9*, const CVector2&);
		static void						DestroyInstance();

		virtual void					Draw();

		virtual VertexBufferPtr			CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		virtual TexturePtr				CreateTextureFromFile(const char*);
		virtual TexturePtr				CreateTextureFromMemory(const void*, uint32);
		virtual TexturePtr				CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32);

		IDirect3DDevice9*				GetDevice() const;

		void							SetFrameRate(float);

	protected:
		typedef std::vector<CMesh*> RenderQueue;
		typedef std::tr1::unordered_map<uint64, IDirect3DVertexDeclaration9*> VertexDeclarationMap;

										CDx9GraphicDevice(IDirect3DDevice9*, const CVector2&);
		virtual							~CDx9GraphicDevice();

		IDirect3DVertexDeclaration9*	CreateVertexDeclaration(const VERTEX_BUFFER_DESCRIPTOR&);

		bool							FillRenderQueue(CSceneNode*, CCamera*);
		void							DrawMesh(CMesh*);

		IDirect3DDevice9*				m_device;
		VertexDeclarationMap			m_vertexDeclarations;
		RenderQueue						m_renderQueue;
	};
}

#endif
