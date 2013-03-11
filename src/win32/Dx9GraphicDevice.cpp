#include <assert.h>
#include <d3dx9.h>
#include "athena/win32/Dx9GraphicDevice.h"
#include "athena/win32/Dx9VertexBuffer.h"
#include "athena/win32/Dx9Texture.h"
#include "athena/win32/Dx9RenderTarget.h"
#include "athena/win32/Dx9CubeRenderTarget.h"
#include "athena/win32/Dx9EffectGenerator.h"
#include "athena/Mesh.h"
#include "athena/MeshProvider.h"

using namespace Athena;

static const D3DCULL g_cullingModes[CULLING_MODE_MAX] =
{
	D3DCULL_NONE,
	D3DCULL_CCW,
	D3DCULL_CW
};

static const D3DTEXTUREADDRESS g_textureAddressModes[TEXTURE_ADDRESS_MODE_MAX] =
{
	D3DTADDRESS_CLAMP,
	D3DTADDRESS_WRAP,
};

static const D3DSTENCILOP g_stencilOp[STENCIL_FAIL_ACTION_MAX] =
{
	D3DSTENCILOP_KEEP,
	D3DSTENCILOP_REPLACE
};

static const D3DCMPFUNC g_stencilFunc[STENCIL_FUNCTION_MAX] =
{
	D3DCMP_NEVER,
	D3DCMP_ALWAYS,
	D3DCMP_EQUAL
};

CDx9GraphicDevice::CDx9GraphicDevice(HWND parentWnd, const CVector2& screenSize)
: m_d3d(NULL)
, m_device(NULL)
, m_parentWnd(parentWnd)
{
	m_screenSize = screenSize;
	m_renderQueue.reserve(0x1000);

	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	assert(m_d3d != NULL);
	CreateDevice();
}

CDx9GraphicDevice::~CDx9GraphicDevice()
{
	for(auto declarationIterator(std::begin(m_vertexDeclarations)); declarationIterator != std::end(m_vertexDeclarations); declarationIterator++)
	{
		IDirect3DVertexDeclaration9* declaration = declarationIterator->second;
		declaration->Release();
	}
	m_vertexDeclarations.clear();

	for(auto effectIterator(std::begin(m_effects)); effectIterator != std::end(m_effects); effectIterator++)
	{
		EFFECTINFO& effectInfo(effectIterator->second);
		effectInfo.effect->Release();
	}
	m_effects.clear();

	m_device->Release();
	m_d3d->Release();
}

void CDx9GraphicDevice::CreateDevice()
{
	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed					= TRUE;
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow				= m_parentWnd;
	d3dpp.BackBufferFormat			= D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth			= static_cast<UINT>(m_screenSize.x);
	d3dpp.BackBufferHeight			= static_cast<UINT>(m_screenSize.y);
	d3dpp.EnableAutoDepthStencil	= TRUE;
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D24S8;

	HRESULT result = m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_parentWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_device);
	assert(SUCCEEDED(result));
}

void CDx9GraphicDevice::CreateInstance(HWND parentWnd, const CVector2& screenSize)
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CDx9GraphicDevice(parentWnd, screenSize);
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

HWND CDx9GraphicDevice::GetParentWindow() const
{
	return m_parentWnd;
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
		auto declarationIterator(m_vertexDeclarations.find(descriptorKey));
		if(declarationIterator == std::end(m_vertexDeclarations))
		{
			vertexDeclaration = CreateVertexDeclaration(bufferDesc);
			m_vertexDeclarations[descriptorKey] = vertexDeclaration;
		}
		else
		{
			vertexDeclaration = declarationIterator->second;
		}
	}
	return std::make_shared<CDx9VertexBuffer>(m_device, bufferDesc, vertexDeclaration);
}

TexturePtr CDx9GraphicDevice::CreateTexture(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return CDx9Texture::Create(m_device, textureFormat, width, height);
}

TexturePtr CDx9GraphicDevice::CreateTextureFromFile(const char* path)
{
	return CDx9Texture::CreateFromFile(m_device, path);
}

TexturePtr CDx9GraphicDevice::CreateTextureFromMemory(const void* data, uint32 dataSize)
{
	return CDx9Texture::CreateFromMemory(m_device, data, dataSize);
}

TexturePtr CDx9GraphicDevice::CreateTextureFromRawData(const void* data, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	auto texture = CDx9Texture::Create(m_device, textureFormat, width, height);
	std::static_pointer_cast<CDx9Texture>(texture)->Update(data);
	return texture;
}

TexturePtr CDx9GraphicDevice::CreateCubeTextureFromFile(const char* path)
{
	return CDx9Texture::CreateCubeFromFile(m_device, path);
}

RenderTargetPtr CDx9GraphicDevice::CreateRenderTarget(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return std::make_shared<CDx9RenderTarget>(m_device, textureFormat, width, height);
}

void CDx9GraphicDevice::UpdateTexture(const TexturePtr& texture, const void* data)
{
	std::static_pointer_cast<CDx9Texture>(texture)->Update(data);
}

CubeRenderTargetPtr CDx9GraphicDevice::CreateCubeRenderTarget(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return std::make_shared<CDx9CubeRenderTarget>(m_device, textureFormat, size);
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
		D3DVERTEXELEMENT9 vertexElement;
		vertexElement.Stream		= 0;
		vertexElement.Offset		= static_cast<WORD>(descriptor.nrmOffset);
		vertexElement.Type			= D3DDECLTYPE_FLOAT3;
		vertexElement.Method		= D3DDECLMETHOD_DEFAULT;
		vertexElement.Usage			= D3DDECLUSAGE_NORMAL;
		vertexElement.UsageIndex	= 0;
		vertexElements.push_back(vertexElement);
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

void CDx9GraphicDevice::GenerateEffect(const CDx9EffectGenerator::EFFECTCAPS& effectCaps)
{
	EFFECTINFO newEffect;
	std::string effectText = CDx9EffectGenerator::GenerateEffect(effectCaps);
	newEffect.effect = CompileEffect(effectText.c_str());

	newEffect.viewProjMatrixHandle		= newEffect.effect->GetParameterByName(NULL, "c_viewProjMatrix");
	newEffect.worldMatrixHandle			= newEffect.effect->GetParameterByName(NULL, "c_worldMatrix");
	newEffect.meshColorHandle			= newEffect.effect->GetParameterByName(NULL, "c_meshColor");
	newEffect.cameraPosHandle			= newEffect.effect->GetParameterByName(NULL, "c_cameraPos");

	for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
	{
		{
			char paramName[256];
			sprintf(paramName, "c_diffuse%dTexture", i);
			newEffect.diffuseTexture[i] = newEffect.effect->GetParameterByName(NULL, paramName);
		}

		{
			char paramName[256];
			sprintf(paramName, "c_diffuse%dTextureMatrix", i);
			newEffect.diffuseTextureMatrix[i] = newEffect.effect->GetParameterByName(NULL, paramName);
		}

		{
			char paramName[256];
			sprintf(paramName, "c_diffuse%dTextureAddressModeU", i);
			newEffect.diffuseTextureAddressModeU[i] = newEffect.effect->GetParameterByName(NULL, paramName);
		}

		{
			char paramName[256];
			sprintf(paramName, "c_diffuse%dTextureAddressModeV", i);
			newEffect.diffuseTextureAddressModeV[i] = newEffect.effect->GetParameterByName(NULL, paramName);
		}
	}

	uint32 effectKey = *reinterpret_cast<const uint32*>(&effectCaps);
	m_effects[effectKey] = newEffect;
}

ID3DXEffect* CDx9GraphicDevice::CompileEffect(const char* text)
{
	ID3DXEffect* effect(NULL);
	ID3DXBuffer* errors(NULL);
	HRESULT result = D3DXCreateEffect(m_device, text, strlen(text), NULL, NULL, D3DXSHADER_OPTIMIZATION_LEVEL3, NULL, &effect, &errors);
	if(FAILED(result))
	{
		std::string errorText(reinterpret_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize());
		OutputDebugStringA("FAILED TO COMPILE EFFECT\r\n");
		OutputDebugStringA("-----------------------------------\r\n");
		OutputDebugStringA(errorText.c_str());
		OutputDebugStringA("-----------------------------------\r\n");
		DebugBreak();
	}
	if(errors != NULL)
	{
		errors->Release();
	}
	return effect;
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
//	m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
	{
		m_device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}

	result = m_device->BeginScene();
	assert(SUCCEEDED(result));

	//Draw all viewports
	for(CViewport* viewport : m_viewports)
	{
		DrawViewport(viewport);
	}

	result = m_device->EndScene();
	assert(SUCCEEDED(result));

	result = m_device->Present(NULL, NULL, NULL, NULL);
	assert(SUCCEEDED(result));
}

void CDx9GraphicDevice::DrawViewportToSurface(IDirect3DSurface9* renderSurface, CViewport* viewport)
{
	IDirect3DSurface9* backBuffer(NULL);
	m_device->GetRenderTarget(0, &backBuffer);
	
	m_device->SetRenderTarget(0, renderSurface);

	HRESULT result = m_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	assert(SUCCEEDED(result));

	{
		result = m_device->BeginScene();
		assert(SUCCEEDED(result));

		DrawViewport(viewport);

		result = m_device->EndScene();
		assert(SUCCEEDED(result));
	}

	m_device->SetRenderTarget(0, backBuffer);

	backBuffer->Release();
}

void CDx9GraphicDevice::DrawViewport(CViewport* viewport)
{
	HRESULT result = m_device->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	assert(SUCCEEDED(result));

	auto camera = viewport->GetCamera();
	assert(camera);

	D3DXMATRIX projMatrix(reinterpret_cast<const float*>(&camera->GetProjectionMatrix()));
	D3DXMATRIX viewMatrix(reinterpret_cast<const float*>(&camera->GetViewMatrix()));

	D3DXMatrixInverse(&m_invViewMatrix, NULL, &viewMatrix);

	m_viewProjMatrix = viewMatrix * projMatrix;

	viewMatrix(3, 0) = 0;
	viewMatrix(3, 1) = 0;
	viewMatrix(3, 2) = 0;

	m_peggedViewProjMatrix = viewMatrix * projMatrix;

	m_renderQueue.clear();

	const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(std::bind(&CDx9GraphicDevice::FillRenderQueue, this, std::placeholders::_1, camera.get()));

	for(CMesh* mesh : m_renderQueue)
	{
		DrawMesh(mesh);
	}
}

bool CDx9GraphicDevice::FillRenderQueue(CSceneNode* node, CCamera* camera)
{
	if(!node->GetVisible()) return false;

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

	D3DXMATRIX worldMatrix(mesh->GetWorldTransformation().coeff);

	const EFFECTINFO* currentEffect(NULL);

	//Setup material
	{
		MaterialPtr material = mesh->GetMaterial();
		assert(material != NULL);
		CColor meshColor = material->GetColor();

		CDx9EffectGenerator::EFFECTCAPS effectCaps;
		memset(&effectCaps, 0, sizeof(effectCaps));

		if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_COLOR)
		{
			effectCaps.hasVertexColor = true;
		}

		for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
		{
			if(material->GetTexture(i))
			{
				effectCaps.setHasDiffuseMap(i, true);
				effectCaps.setDiffuseMapCoordSrc(i, material->GetTextureCoordSource(i));
				if(i != 0)
				{
					unsigned int combineMode = DIFFUSE_MAP_COMBINE_MODULATE;
					switch(material->GetTextureCombineMode(i))
					{
					case TEXTURE_COMBINE_MODULATE:
						combineMode = DIFFUSE_MAP_COMBINE_MODULATE;
						break;
					case TEXTURE_COMBINE_LERP:
						combineMode = DIFFUSE_MAP_COMBINE_LERP;
						break;
					case TEXTURE_COMBINE_ADD:
						combineMode = DIFFUSE_MAP_COMBINE_ADD;
						break;
					}
					effectCaps.setDiffuseMapCombineMode(i, combineMode);
				}
			}
		}

		//Find the proper effect
		uint32 effectKey = *reinterpret_cast<uint32*>(&effectCaps);
		auto effectIterator = m_effects.find(effectKey);
		if(effectIterator == std::end(m_effects))
		{
			GenerateEffect(effectCaps);
			effectIterator = m_effects.find(effectKey);
		}

		currentEffect = &effectIterator->second;

		if(mesh->GetIsPeggedToOrigin())
		{
			currentEffect->effect->SetMatrix(currentEffect->viewProjMatrixHandle, &m_peggedViewProjMatrix);
		}
		else
		{
			currentEffect->effect->SetMatrix(currentEffect->viewProjMatrixHandle, &m_viewProjMatrix);
		}
		currentEffect->effect->SetMatrix(currentEffect->worldMatrixHandle, &worldMatrix);
		currentEffect->effect->SetVector(currentEffect->meshColorHandle, reinterpret_cast<D3DXVECTOR4*>(&meshColor));

		if(currentEffect->cameraPosHandle)
		{
			D3DXVECTOR4 cameraPos = D3DXVECTOR4(m_invViewMatrix(3, 0), m_invViewMatrix(3, 1), m_invViewMatrix(3, 2), 0);
			currentEffect->effect->SetVector(currentEffect->cameraPosHandle, reinterpret_cast<D3DXVECTOR4*>(&cameraPos));
		}

		for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
		{
			if(currentEffect->diffuseTexture[i])
			{
				IDirect3DBaseTexture9* textureHandle = reinterpret_cast<IDirect3DBaseTexture9*>(material->GetTexture(i)->GetHandle());
				const CMatrix4& textureMatrix = material->GetTextureMatrix(i);
				currentEffect->effect->SetTexture(currentEffect->diffuseTexture[i], textureHandle);
				currentEffect->effect->SetMatrix(currentEffect->diffuseTextureMatrix[i], reinterpret_cast<const D3DXMATRIX*>(&textureMatrix));
				currentEffect->effect->SetInt(currentEffect->diffuseTextureAddressModeU[i], g_textureAddressModes[material->GetTextureAddressModeU(i)]);
				currentEffect->effect->SetInt(currentEffect->diffuseTextureAddressModeV[i], g_textureAddressModes[material->GetTextureAddressModeV(i)]);
			}
		}

		CULLING_MODE cullingMode = material->GetCullingMode();
		m_device->SetRenderState(D3DRS_CULLMODE, g_cullingModes[cullingMode]);

		ALPHA_BLENDING_MODE alphaBlendingMode = material->GetAlphaBlendingMode();
		if(alphaBlendingMode == ALPHA_BLENDING_LERP)
		{
			m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else if(alphaBlendingMode == ALPHA_BLENDING_ADD)
		{
			m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}
		else
		{
			m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}

		if(material->GetStencilEnabled())
		{
			m_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
			m_device->SetRenderState(D3DRS_STENCILFAIL, g_stencilOp[material->GetStencilFailAction()]);
			m_device->SetRenderState(D3DRS_STENCILFUNC, g_stencilFunc[material->GetStencilFunction()]);
			m_device->SetRenderState(D3DRS_STENCILREF, material->GetStencilValue());
		}
		else
		{
			m_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		}

		if(mesh->GetIsPeggedToOrigin())
		{
			m_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
		else
		{
			m_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
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

	UINT numPasses = 0;
	currentEffect->effect->Begin(&numPasses, D3DXFX_DONOTSAVESTATE);
	for(UINT i = 0; i < numPasses; i++)
	{
		currentEffect->effect->BeginPass(i);
		{
			HRESULT result = m_device->DrawIndexedPrimitive(primitiveType, 0, 0, descriptor.vertexCount, 0, primCount);
			assert(SUCCEEDED(result));
		}
		currentEffect->effect->EndPass();
	}

	currentEffect->effect->End();

	m_drawCallCount++;
}

D3DFORMAT CDx9GraphicDevice::ConvertTextureFormatId(TEXTURE_FORMAT textureFormat)
{
	D3DFORMAT specTextureFormat = D3DFMT_R8G8B8;
	switch(textureFormat)
	{
	case TEXTURE_FORMAT_RGB888:
		specTextureFormat = D3DFMT_R8G8B8;
		break;
	default:
		assert(0);
		break;
	}
	return specTextureFormat;
}

uint32 CGraphicDevice::ConvertColorToUInt32(const CColor& color)
{
	return D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a);
}
