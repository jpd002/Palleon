#include <assert.h>
#include <d3dx9.h>
#include "athena/win32/Dx9GraphicDevice.h"
#include "athena/win32/Dx9VertexBuffer.h"
#include "athena/win32/Dx9Texture.h"
#include "athena/Mesh.h"
#include "athena/MeshProvider.h"

using namespace Athena;

CDx9GraphicDevice::CDx9GraphicDevice(IDirect3DDevice9* device, const CVector2& screenSize)
: m_device(device)
{
	m_renderQueue.reserve(0x1000);
	m_screenSize = screenSize;
}

CDx9GraphicDevice::~CDx9GraphicDevice()
{
	for(VertexDeclarationMap::const_iterator declarationIterator(m_vertexDeclarations.begin());
		declarationIterator != m_vertexDeclarations.end(); declarationIterator++)
	{
		IDirect3DVertexDeclaration9* declaration = declarationIterator->second;
		declaration->Release();
	}
	m_vertexDeclarations.clear();
}

void CDx9GraphicDevice::CreateInstance(IDirect3DDevice9* device, const CVector2& screenSize)
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CDx9GraphicDevice(device, screenSize);
}

void CDx9GraphicDevice::DestroyInstance()
{
	assert(m_instance != NULL);
	delete m_instance;
	m_instance = NULL;
}

IDirect3DDevice9* CDx9GraphicDevice::GetDevice() const
{
	return m_device;
}

void CDx9GraphicDevice::SetFrameRate(float frameRate)
{
	m_frameRate = frameRate;
}

VertexBufferPtr CDx9GraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& bufferDesc)
{
	IDirect3DVertexDeclaration9* vertexDeclaration(NULL);
	uint64 descriptorKey = bufferDesc.MakeKey();
	{
		VertexDeclarationMap::const_iterator declarationIterator(m_vertexDeclarations.find(descriptorKey));
		if(declarationIterator == m_vertexDeclarations.end())
		{
			vertexDeclaration = CreateVertexDeclaration(bufferDesc);
			m_vertexDeclarations[descriptorKey] = vertexDeclaration;
		}
		else
		{
			vertexDeclaration = declarationIterator->second;
		}
	}
	return VertexBufferPtr(new CDx9VertexBuffer(m_device, bufferDesc, vertexDeclaration));
}

TexturePtr CDx9GraphicDevice::CreateTextureFromFile(const char* path)
{
	return TexturePtr(new CDx9Texture(m_device, path));
}

TexturePtr CDx9GraphicDevice::CreateTextureFromMemory(const void* data, uint32 dataSize)
{
	return TexturePtr(new CDx9Texture(m_device, data, dataSize));
}

TexturePtr CDx9GraphicDevice::CreateTextureFromRawData(const void* data, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return TexturePtr(new CDx9Texture(m_device, data, textureFormat, width, height));
}

IDirect3DVertexDeclaration9* CDx9GraphicDevice::CreateVertexDeclaration(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	typedef std::vector<D3DVERTEXELEMENT9> VertexElementArray;

	VertexElementArray vertexElements;

	uint32 vertexFlags = descriptor.vertexFlags;

	if(vertexFlags & VERTEX_BUFFER_HAS_POS)
	{
		D3DVERTEXELEMENT9 vertexElement;
		vertexElement.Stream		= 0;
		vertexElement.Offset		= static_cast<WORD>(descriptor.posOffset);
		vertexElement.Type			= D3DDECLTYPE_FLOAT3;
		vertexElement.Method		= D3DDECLMETHOD_DEFAULT;
		vertexElement.Usage			= D3DDECLUSAGE_POSITION;
		vertexElement.UsageIndex	= 0;
		vertexElements.push_back(vertexElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_NRM)
	{
		assert(0);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV0)
	{
		D3DVERTEXELEMENT9 vertexElement;
		vertexElement.Stream		= 0;
		vertexElement.Offset		= static_cast<WORD>(descriptor.uv0Offset);
		vertexElement.Type			= D3DDECLTYPE_FLOAT2;
		vertexElement.Method		= D3DDECLMETHOD_DEFAULT;
		vertexElement.Usage			= D3DDECLUSAGE_TEXCOORD;
		vertexElement.UsageIndex	= 0;
		vertexElements.push_back(vertexElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV1)
	{
		D3DVERTEXELEMENT9 vertexElement;
		vertexElement.Stream		= 0;
		vertexElement.Offset		= static_cast<WORD>(descriptor.uv1Offset);
		vertexElement.Type			= D3DDECLTYPE_FLOAT2;
		vertexElement.Method		= D3DDECLMETHOD_DEFAULT;
		vertexElement.Usage			= D3DDECLUSAGE_TEXCOORD;
		vertexElement.UsageIndex	= 1;
		vertexElements.push_back(vertexElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_COLOR)
	{
		D3DVERTEXELEMENT9 vertexElement;
		vertexElement.Stream		= 0;
		vertexElement.Offset		= static_cast<WORD>(descriptor.colorOffset);
		vertexElement.Type			= D3DDECLTYPE_D3DCOLOR;
		vertexElement.Method		= D3DDECLMETHOD_DEFAULT;
		vertexElement.Usage			= D3DDECLUSAGE_COLOR;
		vertexElement.UsageIndex	= 0;
		vertexElements.push_back(vertexElement);
	}

	{
		D3DVERTEXELEMENT9 vertexElement = D3DDECL_END();
		vertexElements.push_back(vertexElement);
	}

	IDirect3DVertexDeclaration9* vertexDeclaration(NULL);
	HRESULT result = m_device->CreateVertexDeclaration(&vertexElements[0], &vertexDeclaration);
	assert(SUCCEEDED(result));

	return vertexDeclaration;
}

void CDx9GraphicDevice::Draw()
{
	HRESULT result;

	result = m_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	assert(SUCCEEDED(result));

	//Reset metrics
	m_drawCallCount = 0;

	m_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); 
//	m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	result = m_device->BeginScene();
	assert(SUCCEEDED(result));

	//Draw all viewports
	for(ViewportList::iterator viewportIterator(m_viewports.begin());
		viewportIterator != m_viewports.end(); viewportIterator++)
	{
		result = m_device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		assert(SUCCEEDED(result));

		CViewport* viewport(*viewportIterator);
		CameraPtr camera = viewport->GetCamera();

		D3DXMATRIX projMatrix(reinterpret_cast<const float*>(&camera->GetProjectionMatrix()));
		D3DXMATRIX viewMatrix(reinterpret_cast<const float*>(&camera->GetViewMatrix()));

		m_device->SetTransform(D3DTS_PROJECTION, &projMatrix);
		m_device->SetTransform(D3DTS_VIEW, &viewMatrix);

		m_renderQueue.clear();

		const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
		sceneRoot->TraverseNodes(std::tr1::bind(&CDx9GraphicDevice::FillRenderQueue, this, std::tr1::placeholders::_1, camera.get()));

		for(RenderQueue::const_iterator meshIterator(m_renderQueue.begin());
			meshIterator != m_renderQueue.end(); meshIterator++)
		{
			CMesh* mesh = (*meshIterator);
			DrawMesh(mesh);
		}
	}

	result = m_device->EndScene();
	assert(SUCCEEDED(result));

	result = m_device->Present(NULL, NULL, NULL, NULL);
	assert(SUCCEEDED(result));
}

bool CDx9GraphicDevice::FillRenderQueue(CSceneNode* node, CCamera* camera)
{
	if(CMesh* mesh = dynamic_cast<CMesh*>(node))
	{
		m_renderQueue.push_back(mesh);
	}
	else if(CMeshProvider* meshProvider = dynamic_cast<CMeshProvider*>(node))
	{
		meshProvider->GetMeshes(m_renderQueue, camera);
	}
	return true;
}

void CDx9GraphicDevice::DrawMesh(CMesh* mesh)
{
	if(mesh->GetPrimitiveCount() == 0) return;

	CDx9VertexBuffer* vertexBufferGen = static_cast<CDx9VertexBuffer*>(mesh->GetVertexBuffer().get());
	assert(vertexBufferGen != NULL);

	const VERTEX_BUFFER_DESCRIPTOR& descriptor = vertexBufferGen->GetDescriptor();
	IDirect3DVertexBuffer9* vertexBuffer = vertexBufferGen->GetVertexBuffer();
	IDirect3DIndexBuffer9* indexBuffer = vertexBufferGen->GetIndexBuffer();
	IDirect3DVertexDeclaration9* vertexDeclaration = vertexBufferGen->GetVertexDeclaration();

	CVector2 worldPosition = mesh->GetWorldPosition();
	CVector2 worldScale = mesh->GetWorldScale();

	D3DXMATRIX worldMatrix;
	D3DXMatrixTranslation(&worldMatrix, worldPosition.x, worldPosition.y, 0);
	worldMatrix.m[0][0] = worldScale.x;
	worldMatrix.m[1][1] = worldScale.y;

	m_device->SetTransform(D3DTS_WORLD, &worldMatrix);

	//Setup material
	{
		CColor color = mesh->GetColor();
		MaterialPtr material = mesh->GetMaterial();
		assert(material != NULL);
		RENDER_TYPE renderType = material->GetRenderType();

		m_device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a));

		m_device->SetTexture(0, NULL);
		m_device->SetTexture(1, NULL);

		unsigned int currentStage = 0;
	
		if(renderType == RENDER_DIFFUSE)
		{
			TexturePtr texture = material->GetTexture(0);

			if(texture)
			{
				CDx9Texture* textureGen = static_cast<CDx9Texture*>(texture.get());

				m_device->SetTexture(currentStage, textureGen->GetTexture());
		
				m_device->SetTextureStageState(currentStage, D3DTSS_COLOROP,	D3DTOP_MODULATE);
				m_device->SetTextureStageState(currentStage, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
				m_device->SetTextureStageState(currentStage, D3DTSS_COLORARG2,	D3DTA_CURRENT);
		
				m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
				m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
				m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAARG2,	D3DTA_CURRENT);
		
				currentStage++;
			}

			m_device->SetTexture(1, NULL);
		}
		else if(material->GetRenderType() == RENDER_LIGHTMAPPED)
		{
			TexturePtr diffuseTexture = material->GetTexture(0);
			TexturePtr lightMapTexture = material->GetTexture(1);

			if(diffuseTexture)
			{
				CDx9Texture* textureGen = static_cast<CDx9Texture*>(diffuseTexture.get());
				m_device->SetTexture(0, textureGen->GetTexture());
			}
			else
			{
				m_device->SetTexture(0, NULL);
			}			

			m_device->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
			m_device->SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
			m_device->SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_CURRENT);
	
			m_device->SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
			m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2,	D3DTA_CURRENT);

			if(lightMapTexture)
			{
				CDx9Texture* textureGen = static_cast<CDx9Texture*>(lightMapTexture.get());
				m_device->SetTexture(1, textureGen->GetTexture());
			}
			else
			{
				m_device->SetTexture(1, NULL);
			}

			m_device->SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_MODULATE);
			m_device->SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
			m_device->SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_CURRENT);
	
			m_device->SetTextureStageState(1, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			m_device->SetTextureStageState(1, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
			m_device->SetTextureStageState(1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT);

			currentStage = 2;
		}

		//Global coloring stage
		{
			m_device->SetTextureStageState(currentStage, D3DTSS_COLOROP,	D3DTOP_MODULATE);
			m_device->SetTextureStageState(currentStage, D3DTSS_COLORARG1,	D3DTA_TFACTOR);
			m_device->SetTextureStageState(currentStage, D3DTSS_COLORARG2,	D3DTA_CURRENT);
	
			m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAARG1,	D3DTA_TFACTOR);
			m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAARG2,	D3DTA_CURRENT);
	
			currentStage++;
		}

		if(material->GetIsTransparent())
		{
			m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else
		{
			m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}

	D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
	UINT primCount = mesh->GetPrimitiveCount();
	switch(mesh->GetPrimitiveType())
	{
	case PRIMITIVE_TRIANGLE_STRIP:
		primitiveType = D3DPT_TRIANGLESTRIP;
		break;
	case PRIMITIVE_TRIANGLE_LIST:
		primitiveType = D3DPT_TRIANGLELIST;
		break;
	default:
		assert(0);
		break;
	}

	m_device->SetVertexDeclaration(vertexDeclaration);
	m_device->SetStreamSource(0, vertexBuffer, 0, descriptor.GetVertexSize());
	m_device->SetIndices(indexBuffer);

	HRESULT result = m_device->DrawIndexedPrimitive(primitiveType, 0, 0, descriptor.vertexCount, 0, primCount);
	assert(SUCCEEDED(result));

	m_drawCallCount++;
}

uint32 CGraphicDevice::ConvertColorToUInt32(const CColor& color)
{
	return D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a);
}
