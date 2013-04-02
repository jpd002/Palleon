#include <assert.h>
#include "athena/win32/Dx11GraphicDevice.h"
#include "athena/win32/Dx11VertexBuffer.h"
#include "athena/win32/Dx11Texture.h"
#include "athena/Mesh.h"
#include "athena/MeshProvider.h"
#include <D3Dcompiler.h>

using namespace Athena;

/*
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
*/

CDx11GraphicDevice::CDx11GraphicDevice(HWND parentWnd, const CVector2& screenSize)
: m_parentWnd(parentWnd)
{
	m_screenSize = screenSize;
	m_renderQueue.reserve(0x1000);

	CreateDevice();
}

CDx11GraphicDevice::~CDx11GraphicDevice()
{
//	for(auto declarationIterator(std::begin(m_vertexDeclarations)); declarationIterator != std::end(m_vertexDeclarations); declarationIterator++)
//	{
//		IDirect3DVertexDeclaration9* declaration = declarationIterator->second;
//		declaration->Release();
//	}
//	m_vertexDeclarations.clear();

	for(auto& effectInfoPair : m_effects)
	{
		EFFECTINFO& effectInfo(effectInfoPair.second);
		effectInfo.vertexShader->Release();
		effectInfo.pixelShader->Release();
		effectInfo.constantBuffer->Release();
		effectInfo.vertexShaderCode->Release();
	}
	m_effects.clear();
}

void CDx11GraphicDevice::CreateDevice()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	HRESULT result = S_OK;

	swapChainDesc.BufferCount							= 1;
	swapChainDesc.BufferDesc.Width						= static_cast<UINT>(m_screenSize.x);
	swapChainDesc.BufferDesc.Height						= static_cast<UINT>(m_screenSize.y);
	swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow							= m_parentWnd;
	swapChainDesc.SampleDesc.Count						= 1;
	swapChainDesc.SampleDesc.Quality					= 0;
	swapChainDesc.Windowed								= true;
	swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags									= 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT deviceCreationFlags = 0;
#ifdef _DEBUG
	deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceCreationFlags, &featureLevel, 1, D3D11_SDK_VERSION, 
		&swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	assert(SUCCEEDED(result));
	
	{
		Framework::Win32::CComPtr<ID3D11Texture2D> backBuffer;
		result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		assert(SUCCEEDED(result));
	
		result = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
		assert(SUCCEEDED(result));
	}

	{
		D3D11_TEXTURE2D_DESC depthBufferDesc = {};

		depthBufferDesc.Width				= static_cast<UINT>(m_screenSize.x);
		depthBufferDesc.Height				= static_cast<UINT>(m_screenSize.y);
		depthBufferDesc.MipLevels			= 1;
		depthBufferDesc.ArraySize			= 1;
		depthBufferDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count	= 1;
		depthBufferDesc.SampleDesc.Quality	= 0;
		depthBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags		= 0;
		depthBufferDesc.MiscFlags			= 0;

		result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthBuffer);
		assert(SUCCEEDED(result));
	}

	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		depthStencilDesc.DepthEnable		= false;
		depthStencilDesc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc			= D3D11_COMPARISON_LESS;

		result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		assert(SUCCEEDED(result));
	}

	{
		D3D11_RASTERIZER_DESC rasterDesc = {};

		rasterDesc.CullMode					= D3D11_CULL_NONE;
		rasterDesc.FrontCounterClockwise	= false;
		rasterDesc.DepthClipEnable			= true;
		rasterDesc.FillMode					= D3D11_FILL_SOLID;

		result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
		assert(SUCCEEDED(result));
	}

	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU	= D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV	= D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW	= D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter		= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		
		result = m_device->CreateSamplerState(&samplerDesc, &m_defaultSamplerState);
		assert(SUCCEEDED(result));
	}

	m_deviceContext->RSSetState(m_rasterState);
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
}

void CDx11GraphicDevice::CreateInstance(HWND parentWnd, const CVector2& screenSize)
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CDx11GraphicDevice(parentWnd, screenSize);
}

void CDx11GraphicDevice::DestroyInstance()
{
	assert(m_instance != NULL);
	delete m_instance;
	m_instance = NULL;
}
/*
IDirect3DDevice9* CDx11GraphicDevice::GetDevice() const
{
	return m_device;
}
*/
HWND CDx11GraphicDevice::GetParentWindow() const
{
	return m_parentWnd;
}

void CDx11GraphicDevice::SetFrameRate(float frameRate)
{
	m_frameRate = frameRate;
}

VertexBufferPtr CDx11GraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& bufferDesc)
{
	return std::make_shared<CDx11VertexBuffer>(m_device, m_deviceContext, bufferDesc);
}

TexturePtr CDx11GraphicDevice::CreateTexture(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return CDx11Texture::Create(m_device, textureFormat, width, height);
}

TexturePtr CDx11GraphicDevice::CreateTextureFromFile(const char* path)
{
	return CDx11Texture::CreateFromFile(m_device, path);
}

TexturePtr CDx11GraphicDevice::CreateTextureFromMemory(const void* data, uint32 dataSize)
{
	return TexturePtr();
}

TexturePtr CDx11GraphicDevice::CreateTextureFromRawData(const void* data, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return TexturePtr();
}

TexturePtr CDx11GraphicDevice::CreateCubeTextureFromFile(const char* path)
{
	return TexturePtr();
}

RenderTargetPtr CDx11GraphicDevice::CreateRenderTarget(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return RenderTargetPtr();
}

void CDx11GraphicDevice::UpdateTexture(const TexturePtr& texture, const void* data)
{

}

CubeRenderTargetPtr CDx11GraphicDevice::CreateCubeRenderTarget(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return CubeRenderTargetPtr();
}

ID3D11InputLayout* CDx11GraphicDevice::CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR& descriptor, ID3DBlob* vertexShaderCode)
{
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

	InputElementArray inputElements;

	uint32 vertexFlags = descriptor.vertexFlags;

	if(vertexFlags & VERTEX_BUFFER_HAS_POS)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "POSITION";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.posOffset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_NRM)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "NORMAL";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.nrmOffset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV0)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.uv0Offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV1)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 1;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.uv1Offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_COLOR)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "COLOR";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.colorOffset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	ID3D11InputLayout* inputLayout(nullptr);
	HRESULT result = m_device->CreateInputLayout(inputElements.data(), inputElements.size(), 
		vertexShaderCode->GetBufferPointer(), vertexShaderCode->GetBufferSize(), &inputLayout);
	assert(SUCCEEDED(result));

	return inputLayout;
}

void CDx11GraphicDevice::GenerateEffect(const CDx11EffectGenerator::EFFECTCAPS& effectCaps)
{
	EFFECTINFO newEffect = {};
	std::string vertexShaderText = CDx11EffectGenerator::GenerateVertexShader(effectCaps);
	std::string pixelShaderText = CDx11EffectGenerator::GeneratePixelShader(effectCaps);

	HRESULT result = S_OK;

	{
		Framework::Win32::CComPtr<ID3DBlob> vertexShaderCode;
		Framework::Win32::CComPtr<ID3DBlob> vertexShaderErrors;

		result = D3DCompile(vertexShaderText.c_str(), vertexShaderText.length() + 1, "vs", nullptr, nullptr, "VertexProgram", 
			"vs_5_0", D3DCOMPILE_DEBUG, 0, &vertexShaderCode, &vertexShaderErrors);
		if(FAILED(result))
		{
			if(!vertexShaderErrors.IsEmpty())
			{
				OutputDebugStringA(vertexShaderText.c_str());
				OutputDebugStringA("\r\n");
				OutputDebugStringA(reinterpret_cast<const char*>(vertexShaderErrors->GetBufferPointer()));
			}
			DebugBreak();
		}

		result = m_device->CreateVertexShader(vertexShaderCode->GetBufferPointer(), vertexShaderCode->GetBufferSize(), nullptr, &newEffect.vertexShader);
		assert(SUCCEEDED(result));

		newEffect.vertexShaderCode = vertexShaderCode.Detach();
	}

	{
		Framework::Win32::CComPtr<ID3DBlob> pixelShaderCode;
		Framework::Win32::CComPtr<ID3DBlob> pixelShaderErrors;

		result = D3DCompile(pixelShaderText.c_str(), pixelShaderText.length() + 1, "ps", nullptr, nullptr, "PixelProgram",
			"ps_5_0", D3DCOMPILE_DEBUG, 0, &pixelShaderCode, &pixelShaderErrors);
		if(FAILED(result))
		{
			if(!pixelShaderErrors.IsEmpty())
			{
				OutputDebugStringA(pixelShaderText.c_str());
				OutputDebugStringA("\r\n");
				OutputDebugStringA(reinterpret_cast<const char*>(pixelShaderErrors->GetBufferPointer()));
			}
			DebugBreak();
		}

		result = m_device->CreatePixelShader(pixelShaderCode->GetBufferPointer(), pixelShaderCode->GetBufferSize(), nullptr, &newEffect.pixelShader);
		assert(SUCCEEDED(result));
	}

	struct OffsetKeeper
	{
		OffsetKeeper()
		{
			currentOffset = 0;
		}

		uint32 Allocate(uint32 size)
		{
			uint32 result = currentOffset;
			currentOffset += size;
			return result;
		}

		uint32 currentOffset;
	};

	OffsetKeeper constantOffset;

	newEffect.meshColorOffset			= constantOffset.Allocate(0x10);
	newEffect.worldMatrixOffset			= constantOffset.Allocate(0x40);
	newEffect.viewProjMatrixOffset		= constantOffset.Allocate(0x40);
	if(effectCaps.hasDiffuseMap0) newEffect.diffuseTextureMatrixOffset[0] = constantOffset.Allocate(0x40);

	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth		= constantOffset.currentOffset;
		bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

		result = m_device->CreateBuffer(&bufferDesc, nullptr, &newEffect.constantBuffer);
		assert(SUCCEEDED(result));
	}

	uint32 effectKey = *reinterpret_cast<const uint32*>(&effectCaps);
	m_effects[effectKey] = newEffect;
}

ID3D11BlendState* CDx11GraphicDevice::GetBlendState(ALPHA_BLENDING_MODE blendingMode)
{
	if(m_blendStates[blendingMode].IsEmpty())
	{
		D3D11_BLEND_DESC blendDesc = {};
		switch(blendingMode)
		{
		case ALPHA_BLENDING_LERP:
			blendDesc.RenderTarget[0].BlendEnable		= true;
			blendDesc.RenderTarget[0].BlendOp			= D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend			= D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend			= D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOpAlpha		= D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha		= D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ZERO;
			break;
		}
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT result = m_device->CreateBlendState(&blendDesc, &m_blendStates[blendingMode]);
		assert(SUCCEEDED(result));
	}

	return m_blendStates[blendingMode];
}

void CDx11GraphicDevice::Draw()
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width		= m_screenSize.x;
	viewport.Height		= m_screenSize.y;
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;
	viewport.TopLeftX	= 0.0f;
	viewport.TopLeftY	= 0.0f;

	m_deviceContext->RSSetViewports(1, &viewport);

	static const float clearColor[4] = { 0, 0, 0, 0 };

	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	//Draw all viewports
	for(CViewport* viewport : m_viewports)
	{
		DrawViewport(viewport);
	}

	m_swapChain->Present(0, 0);
}

void CDx11GraphicDevice::DrawViewport(CViewport* viewport)
{
	auto camera = viewport->GetCamera();
	assert(camera);

	const auto& projMatrix(camera->GetProjectionMatrix());
	const auto& viewMatrix(camera->GetViewMatrix());

	m_viewProjMatrix = viewMatrix * projMatrix;

	m_renderQueue.clear();

	const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes([&] (CSceneNode* node) { return FillRenderQueue(node, camera.get()); });

	for(CMesh* mesh : m_renderQueue)
	{
		DrawMesh(mesh);
	}
}

bool CDx11GraphicDevice::FillRenderQueue(CSceneNode* node, CCamera* camera)
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

void CDx11GraphicDevice::DrawMesh(CMesh* mesh)
{
	if(mesh->GetPrimitiveCount() == 0) return;

	HRESULT result = S_OK;

	auto vertexBufferGen = std::static_pointer_cast<CDx11VertexBuffer>(mesh->GetVertexBuffer());
	assert(vertexBufferGen);

	const VERTEX_BUFFER_DESCRIPTOR& descriptor = vertexBufferGen->GetDescriptor();
	auto vertexBuffer = vertexBufferGen->GetVertexBuffer();
	auto indexBuffer = vertexBufferGen->GetIndexBuffer();

	const auto& worldMatrix(mesh->GetWorldTransformation());

	EFFECTINFO* currentEffect(nullptr);

	//Setup material
	{
		auto material = mesh->GetMaterial();
		assert(material != NULL);
		CColor meshColor = material->GetColor();

		CDx11EffectGenerator::EFFECTCAPS effectCaps;
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

		ID3D11InputLayout* inputLayout(nullptr);
		uint64 descriptorKey = descriptor.MakeKey();
		{
			auto inputLayoutIterator(currentEffect->inputLayouts.find(descriptorKey));
			if(inputLayoutIterator == std::end(currentEffect->inputLayouts))
			{
				inputLayout = CreateInputLayout(descriptor, currentEffect->vertexShaderCode);
				currentEffect->inputLayouts[descriptorKey] = inputLayout;
			}
			else
			{
				inputLayout = inputLayoutIterator->second;
			}
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource = {};
			result = m_deviceContext->Map(currentEffect->constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			assert(SUCCEEDED(result));

			auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
			*reinterpret_cast<CColor*>(constantBufferPtr + currentEffect->meshColorOffset) = meshColor;
			*reinterpret_cast<CMatrix4*>(constantBufferPtr + currentEffect->worldMatrixOffset) = worldMatrix;
			*reinterpret_cast<CMatrix4*>(constantBufferPtr + currentEffect->viewProjMatrixOffset) = m_viewProjMatrix;

			for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
			{
				if(currentEffect->diffuseTextureMatrixOffset[i] != 0)
				{
					const auto& textureMatrix = material->GetTextureMatrix(i);
					*reinterpret_cast<CMatrix4*>(constantBufferPtr + currentEffect->diffuseTextureMatrixOffset[i]) = textureMatrix;
				}
			}

			m_deviceContext->Unmap(currentEffect->constantBuffer, 0);
		}

		unsigned int textureCount = 0;
		ID3D11ShaderResourceView* textureViews[MAX_DIFFUSE_SLOTS] = {};
		ID3D11SamplerState* samplerStates[MAX_DIFFUSE_SLOTS] = {};
		for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
		{
			auto texture = material->GetTexture(i);
			if(texture)
			{
				auto dx11Texture = std::static_pointer_cast<CDx11Texture>(texture);
				samplerStates[textureCount] = m_defaultSamplerState;
				textureViews[textureCount] = dx11Texture->GetTextureView();
				textureCount++;
			}
		}

		m_deviceContext->IASetInputLayout(inputLayout);
		m_deviceContext->VSSetConstantBuffers(0, 1, &currentEffect->constantBuffer);
		m_deviceContext->VSSetShader(currentEffect->vertexShader, nullptr, 0);
		m_deviceContext->PSSetShader(currentEffect->pixelShader, nullptr, 0);
		m_deviceContext->PSSetShaderResources(0, textureCount, textureViews);
		m_deviceContext->PSSetSamplers(0, textureCount, samplerStates);

		auto blendState = GetBlendState(material->GetAlphaBlendingMode());
		m_deviceContext->OMSetBlendState(blendState, nullptr, ~0);
/*
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
		*/
	}

	UINT vertexStride = vertexBufferGen->GetDescriptor().GetVertexSize();
	UINT vertexOffset = 0;

	m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
	m_deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	UINT primCount = mesh->GetPrimitiveCount();
	UINT indexCount = 0;
	switch(mesh->GetPrimitiveType())
	{
	case PRIMITIVE_TRIANGLE_STRIP:
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		indexCount = (primCount + 2);
		break;
	case PRIMITIVE_TRIANGLE_LIST:
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		indexCount = primCount * 3;
		break;
	default:
		assert(0);
		break;
	}

	m_deviceContext->DrawIndexed(indexCount, 0, 0);

	m_drawCallCount++;
}

uint32 CGraphicDevice::ConvertColorToUInt32(const CColor& color)
{
	return 
		(static_cast<uint32>(color.a * 255.f) << 24) |
		(static_cast<uint32>(color.r * 255.f) <<  0) |
		(static_cast<uint32>(color.g * 255.f) <<  8) |
		(static_cast<uint32>(color.b * 255.f) << 16);
}
