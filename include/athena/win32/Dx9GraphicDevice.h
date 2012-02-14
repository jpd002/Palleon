#ifndef _DX9GRAPHICDEVICE_H_
#define _DX9GRAPHICDEVICE_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <unordered_map>
#include "../GraphicDevice.h"
#include "../Mesh.h"

namespace Athena
{
	class CDx9GraphicDevice : public CGraphicDevice
	{
	public:
		static void						CreateInstance(HWND, const CVector2&);
		static void						DestroyInstance();

		virtual void					Draw();

		virtual VertexBufferPtr			CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		virtual TexturePtr				CreateTextureFromFile(const char*);
		virtual TexturePtr				CreateTextureFromMemory(const void*, uint32);
		virtual TexturePtr				CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32);

		virtual TexturePtr				CreateCubeTextureFromFile(const char*);

		IDirect3DDevice9*				GetDevice() const;
		HWND							GetParentWindow() const;

		void							SetFrameRate(float);

	protected:
		enum
		{
			MAX_DIFFUSE_SLOTS = 5,
		};

		struct EFFECTINFO
		{
			ID3DXEffect*	effect;

			D3DXHANDLE		viewProjMatrixHandle;
			D3DXHANDLE		worldMatrixHandle;
			D3DXHANDLE		meshColorHandle;
			D3DXHANDLE		cameraPosHandle;

			D3DXHANDLE		diffuseTexture[MAX_DIFFUSE_SLOTS];
			D3DXHANDLE		diffuseTextureMatrix[MAX_DIFFUSE_SLOTS];
		};

		typedef std::vector<CMesh*> RenderQueue;
		typedef std::unordered_map<uint32, EFFECTINFO> EffectMap;
		typedef std::unordered_map<uint64, IDirect3DVertexDeclaration9*> VertexDeclarationMap;

										CDx9GraphicDevice(HWND, const CVector2&);
		virtual							~CDx9GraphicDevice();

		void							CreateDevice();
		IDirect3DVertexDeclaration9*	CreateVertexDeclaration(const VERTEX_BUFFER_DESCRIPTOR&);

		ID3DXEffect*					CompileEffect(const char*);

		bool							FillRenderQueue(CSceneNode*, CCamera*);
		void							DrawMesh(CMesh*);

		HWND							m_parentWnd;
		IDirect3D9*						m_d3d;
		IDirect3DDevice9*				m_device;
		VertexDeclarationMap			m_vertexDeclarations;
		EffectMap						m_effects;
		RenderQueue						m_renderQueue;

		D3DXVECTOR4						m_cameraPos;
		D3DXMATRIX						m_viewProjMatrix;
	};
}

#endif
