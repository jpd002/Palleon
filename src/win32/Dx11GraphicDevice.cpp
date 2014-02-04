#include <assert.h>
#include "athena/win32/Dx11GraphicDevice.h"
#include "athena/win32/Dx11VertexBuffer.h"
#include "athena/win32/Dx11Texture.h"
#include "athena/win32/Dx11UberEffectProvider.h"
#include "athena/win32/Dx11ShadowMapEffect.h"
#include "athena/Mesh.h"
#include "athena/MeshProvider.h"
#include "athena/MathOps.h"

#define SHADOW_MAP_SIZE		2048

using namespace Athena;

CDx11GraphicDevice::CDx11GraphicDevice(HWND parentWnd, const CVector2& screenSize)
: m_parentWnd(parentWnd)
{
	m_screenSize = screenSize;
	m_renderQueue.reserve(0x1000);

	CreateDevice();
	CreateShadowMap();
	m_defaultEffectProvider = std::make_shared<CDx11UberEffectProvider>(m_device, m_deviceContext);
	m_shadowMapEffect = std::make_shared<CDx11ShadowMapEffect>(m_device, m_deviceContext);
}

CDx11GraphicDevice::~CDx11GraphicDevice()
{
	m_shadowMapEffect.reset();
	m_defaultEffectProvider.reset();
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

		result = m_device->CreateDepthStencilView(m_depthBuffer, nullptr, &m_depthBufferView);
		assert(SUCCEEDED(result));
	}

	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU	= D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV	= D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW	= D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter		= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.MaxLOD		= FLT_MAX;
		
		result = m_device->CreateSamplerState(&samplerDesc, &m_defaultSamplerState);
		assert(SUCCEEDED(result));
	}
}

void CDx11GraphicDevice::CreateShadowMap()
{
	HRESULT result = S_OK;

	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width					= SHADOW_MAP_SIZE;
		textureDesc.Height					= SHADOW_MAP_SIZE;
		textureDesc.MipLevels				= 1;
		textureDesc.ArraySize				= 1;
		textureDesc.Format					= DXGI_FORMAT_R32_FLOAT;
		textureDesc.SampleDesc.Count		= 1;
		textureDesc.SampleDesc.Quality		= 0;
		textureDesc.Usage					= D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags				= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags			= 0;
		textureDesc.MiscFlags				= 0;

		result = m_device->CreateTexture2D(&textureDesc, nullptr, &m_shadowMap);
		assert(SUCCEEDED(result));

		result = m_device->CreateRenderTargetView(m_shadowMap, nullptr, &m_shadowMapRenderView);
		assert(SUCCEEDED(result));

		result = m_device->CreateShaderResourceView(m_shadowMap, nullptr, &m_shadowMapView);
		assert(SUCCEEDED(result));
	}

	{
		D3D11_TEXTURE2D_DESC depthBufferDesc = {};

		depthBufferDesc.Width				= SHADOW_MAP_SIZE;
		depthBufferDesc.Height				= SHADOW_MAP_SIZE;
		depthBufferDesc.MipLevels			= 1;
		depthBufferDesc.ArraySize			= 1;
		depthBufferDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count	= 1;
		depthBufferDesc.SampleDesc.Quality	= 0;
		depthBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags		= 0;
		depthBufferDesc.MiscFlags			= 0;

		result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_shadowDepthMap);
		assert(SUCCEEDED(result));

		result = m_device->CreateDepthStencilView(m_shadowDepthMap, nullptr, &m_shadowDepthMapView);
		assert(SUCCEEDED(result));
	}
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

ID3D11Device* CDx11GraphicDevice::GetDevice() const
{
	return m_device;
}

ID3D11DeviceContext* CDx11GraphicDevice::GetDeviceContext() const
{
	return m_deviceContext;
}

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

TexturePtr CDx11GraphicDevice::CreateTexture(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height, uint32 mipCount)
{
	return CDx11Texture::Create(m_device, m_deviceContext, textureFormat, width, height, mipCount);
}

TexturePtr CDx11GraphicDevice::CreateCubeTexture(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return CDx11Texture::CreateCube(m_device, m_deviceContext, textureFormat, size);
}

TexturePtr CDx11GraphicDevice::CreateTextureFromFile(const char* path)
{
	return CDx11Texture::CreateFromFile(m_device, m_deviceContext, path);
}

TexturePtr CDx11GraphicDevice::CreateTextureFromMemory(const void* data, uint32 dataSize)
{
	return CDx11Texture::CreateFromMemory(m_device, m_deviceContext, data, dataSize);
}

TexturePtr CDx11GraphicDevice::CreateCubeTextureFromFile(const char* path)
{
	return CDx11Texture::CreateCubeFromFile(m_device, m_deviceContext, path);
}

RenderTargetPtr CDx11GraphicDevice::CreateRenderTarget(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return RenderTargetPtr();
}

CubeRenderTargetPtr CDx11GraphicDevice::CreateCubeRenderTarget(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return CubeRenderTargetPtr();
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
		case ALPHA_BLENDING_ADD:
			blendDesc.RenderTarget[0].BlendEnable		= true;
			blendDesc.RenderTarget[0].BlendOp			= D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend			= D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend			= D3D11_BLEND_ONE;
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

ID3D11RasterizerState* CDx11GraphicDevice::GetRasterizerState(const RASTERIZER_STATE_INFO& stateInfo)
{
	D3D11RasterizerStatePtr state;

	uint32 stateKey = *reinterpret_cast<const uint32*>(&stateInfo);
	auto stateIterator = m_rasterizerStates.find(stateKey);
	if(stateIterator == std::end(m_rasterizerStates))
	{
		static const D3D11_CULL_MODE c_cullModes[CULLING_MODE_MAX] =
		{
			D3D11_CULL_NONE,
			D3D11_CULL_BACK,
			D3D11_CULL_FRONT
		};

		D3D11_RASTERIZER_DESC rasterDesc = {};

		rasterDesc.CullMode					= c_cullModes[stateInfo.cullMode];
		rasterDesc.FrontCounterClockwise	= false;
		rasterDesc.FillMode					= D3D11_FILL_SOLID;
		rasterDesc.DepthClipEnable			= true;

		HRESULT result = m_device->CreateRasterizerState(&rasterDesc, &state);
		assert(SUCCEEDED(result));

		m_rasterizerStates.insert(std::make_pair(stateKey, state));
	}
	else
	{
		state = stateIterator->second;
	}

	return state;
}

ID3D11DepthStencilState* CDx11GraphicDevice::GetDepthStencilState(const DEPTHSTENCIL_STATE_INFO& stateInfo)
{
	D3D11DepthStencilStatePtr state;

	uint32 stateKey = *reinterpret_cast<const uint32*>(&stateInfo);
	auto stateIterator = m_depthStencilStates.find(stateKey);
	if(stateIterator == std::end(m_depthStencilStates))
	{
		static const D3D11_COMPARISON_FUNC c_stencilFunctions[STENCIL_FUNCTION_MAX] =
		{
			D3D11_COMPARISON_NEVER,
			D3D11_COMPARISON_ALWAYS,
			D3D11_COMPARISON_EQUAL
		};

		static const D3D11_STENCIL_OP c_stencilFailOps[STENCIL_FAIL_ACTION_MAX] =
		{
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_REPLACE
		};

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		
		depthStencilDesc.DepthEnable					= TRUE;
		depthStencilDesc.DepthFunc						= D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.DepthWriteMask					= stateInfo.depthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

		depthStencilDesc.StencilEnable					= stateInfo.stencilTestEnabled;
		depthStencilDesc.StencilReadMask				= ~0;
		depthStencilDesc.StencilWriteMask				= ~0;

		depthStencilDesc.FrontFace.StencilFunc			= c_stencilFunctions[stateInfo.stencilFunction];
		depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFailOp		= c_stencilFailOps[stateInfo.stencilFailAction];
		depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;

		depthStencilDesc.BackFace.StencilFunc			= c_stencilFunctions[stateInfo.stencilFunction];
		depthStencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFailOp			= c_stencilFailOps[stateInfo.stencilFailAction];
		depthStencilDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;

		HRESULT result = m_device->CreateDepthStencilState(&depthStencilDesc, &state);
		assert(SUCCEEDED(result));

		m_depthStencilStates.insert(std::make_pair(stateKey, state));
	}
	else
	{
		state = stateIterator->second;
	}

	return state;
}

ID3D11SamplerState* CDx11GraphicDevice::GetSamplerState(const SAMPLER_STATE_INFO& stateInfo)
{
	D3D11SamplerStatePtr state;

	uint32 stateKey = *reinterpret_cast<const uint32*>(&stateInfo);
	auto stateIterator = m_samplerStates.find(stateKey);
	if(stateIterator == std::end(m_samplerStates))
	{
		static const D3D11_TEXTURE_ADDRESS_MODE c_addressMode[TEXTURE_ADDRESS_MODE_MAX] =
		{
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_WRAP
		};

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU	= c_addressMode[stateInfo.addressU];
		samplerDesc.AddressV	= c_addressMode[stateInfo.addressV];
		samplerDesc.AddressW	= D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter		= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.MinLOD		= 0;
		samplerDesc.MaxLOD		= D3D11_FLOAT32_MAX;
		
		HRESULT result = m_device->CreateSamplerState(&samplerDesc, &state);
		assert(SUCCEEDED(result));

		m_samplerStates.insert(std::make_pair(stateKey, state));
	}
	else
	{
		state = stateIterator->second;
	}

	return state;
}

void CDx11GraphicDevice::Draw()
{
	//Reset metrics
	m_drawCallCount = 0;

	static const float clearColor[4] = { 0, 0, 0, 0 };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);

	ID3D11ShaderResourceView* textureViews[MAX_PIXEL_SHADER_RESOURCE_SLOTS] = {};
	ID3D11SamplerState* samplerStates[MAX_PIXEL_SHADER_RESOURCE_SLOTS] = {};
	m_deviceContext->PSSetShaderResources(0, MAX_PIXEL_SHADER_RESOURCE_SLOTS, textureViews);
	m_deviceContext->PSSetSamplers(0, MAX_PIXEL_SHADER_RESOURCE_SLOTS, samplerStates);

	//Draw all viewports
	for(const auto& viewport : m_viewports)
	{
		DrawViewport(viewport);
	}

	m_swapChain->Present(0, 0);
}

void CDx11GraphicDevice::DrawViewport(CViewport* viewport)
{
	DrawViewportShadowMap(viewport);
	DrawViewportMainMap(viewport);
}

void CDx11GraphicDevice::DrawViewportMainMap(CViewport* viewport)
{
	{
		D3D11_VIEWPORT viewport = {};
		viewport.Width		= m_screenSize.x;
		viewport.Height		= m_screenSize.y;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;

		m_deviceContext->RSSetViewports(1, &viewport);
	}

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthBufferView);
	m_deviceContext->ClearDepthStencilView(m_depthBufferView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	auto camera = viewport->GetCamera();
	assert(camera);
	auto cameraFrustum = camera->GetFrustum();
	auto shadowCamera = viewport->GetShadowCamera();

	m_renderQueue.clear();

	const auto& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(
		[&] (const SceneNodePtr& node)
		{
			if(!node->GetVisible()) return false;

			if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
			{
				if(mesh->GetPrimitiveCount() != 0)
				{
					bool render = true;
					auto boundingSphere = mesh->GetBoundingSphere();
					if(boundingSphere.radius != 0)
					{
						auto worldBoundingSphere = mesh->GetWorldBoundingSphere();
						render = cameraFrustum.Intersects(worldBoundingSphere);
					}
					if(render)
					{
						m_renderQueue.push_back(mesh.get());
					}
				}
			}
			else if(auto meshProvider = std::dynamic_pointer_cast<CMeshProvider>(node))
			{
				meshProvider->GetMeshes(m_renderQueue, camera.get());
			}
			return true;
		}
	);

	auto viewMatrix = camera->GetViewMatrix();
	auto projMatrix = camera->GetProjectionMatrix();
	auto shadowViewProjMatrix = shadowCamera ? (shadowCamera->GetViewMatrix() * shadowCamera->GetProjectionMatrix()) : CMatrix4::MakeIdentity();
	bool hasShadowMap = shadowCamera != nullptr;

	auto peggedViewMatrix = camera->GetViewMatrix();
	peggedViewMatrix(3, 0) = 0;
	peggedViewMatrix(3, 1) = 0;
	peggedViewMatrix(3, 2) = 0;

	for(const auto& mesh : m_renderQueue)
	{
		auto effectProvider = mesh->GetEffectProvider();
		auto effect = std::static_pointer_cast<CDx11Effect>(effectProvider->GetEffectForRenderable(mesh, hasShadowMap));
		bool isPeggedToOrigin = mesh->GetIsPeggedToOrigin();
		DrawMesh(mesh, effect, isPeggedToOrigin ? peggedViewMatrix : viewMatrix, projMatrix, hasShadowMap, shadowViewProjMatrix);
	}
}

void CDx11GraphicDevice::DrawViewportShadowMap(CViewport* viewport)
{
	auto camera = viewport->GetShadowCamera();
	if(!camera) return;

	{
		D3D11_VIEWPORT viewport = {};
		viewport.Width		= SHADOW_MAP_SIZE;
		viewport.Height		= SHADOW_MAP_SIZE;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;

		m_deviceContext->RSSetViewports(1, &viewport);
	}

	static const float clearColor[4] = { 1, 1, 1, 1 };
	m_deviceContext->ClearRenderTargetView(m_shadowMapRenderView, clearColor);
	m_deviceContext->ClearDepthStencilView(m_shadowDepthMapView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_deviceContext->OMSetRenderTargets(1, &m_shadowMapRenderView, m_shadowDepthMapView);

	m_renderQueue.clear();

	const auto& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(
		[&] (const SceneNodePtr& node) 
		{
			if(!node->GetVisible()) return false;

			if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
			{
				if(mesh->GetMaterial()->GetShadowCasting())
				{
					m_renderQueue.push_back(mesh.get());
				}
			}
			return true;
		}
	);

	auto viewMatrix = camera->GetViewMatrix();
	auto projMatrix = camera->GetProjectionMatrix();
	for(const auto& mesh : m_renderQueue)
	{
		DrawMesh(mesh, m_shadowMapEffect, viewMatrix, projMatrix);
	}
}

void CDx11GraphicDevice::DrawMesh(CMesh* mesh, const Dx11EffectPtr& effect, const CMatrix4& viewMatrix, const CMatrix4& projMatrix,
	bool hasShadowMap, const CMatrix4& shadowViewProjMatrix)
{
	if(mesh->GetPrimitiveCount() == 0) return;

	HRESULT result = S_OK;

	assert(effect);

	auto vertexBufferGen = std::static_pointer_cast<CDx11VertexBuffer>(mesh->GetVertexBuffer());
	assert(vertexBufferGen);

	const auto& descriptor = vertexBufferGen->GetDescriptor();
	auto vertexBuffer = vertexBufferGen->GetVertexBuffer();
	auto indexBuffer = vertexBufferGen->GetIndexBuffer();
	auto inputLayout = effect->GetInputLayout(descriptor);

	auto material = mesh->GetMaterial();
	assert(material);

	//Setup material
	{
		effect->UpdateConstants(material, mesh->GetWorldTransformation(), viewMatrix, projMatrix, shadowViewProjMatrix);

		//TODO: This needs to be moved in the effect class
		unsigned int textureCount = 0;
		ID3D11ShaderResourceView* textureViews[MAX_PIXEL_SHADER_RESOURCE_SLOTS] = {};
		ID3D11SamplerState* samplerStates[MAX_PIXEL_SHADER_RESOURCE_SLOTS] = {};
		for(unsigned int i = 0; i < CMaterial::MAX_TEXTURE_SLOTS; i++)
		{
			auto texture = material->GetTexture(i);
			if(texture)
			{
				SAMPLER_STATE_INFO samplerStateInfo = {};
				samplerStateInfo.addressU = material->GetTextureAddressModeU(i);
				samplerStateInfo.addressV = material->GetTextureAddressModeV(i);
				auto samplerState = GetSamplerState(samplerStateInfo);

				auto specTexture = std::static_pointer_cast<CDx11Texture>(texture);
				samplerStates[i] = samplerState;
				textureViews[i] = specTexture->GetTextureView();
				textureCount++;
			}
			else
			{
				samplerStates[i] = nullptr;
				textureViews[i] = nullptr;
			}
		}

		if(hasShadowMap)
		{
			assert(samplerStates[textureCount] == nullptr);
			assert(textureViews[textureCount] == nullptr);
			samplerStates[textureCount] = m_defaultSamplerState;
			textureViews[textureCount] = m_shadowMapView;
			textureCount++;
		}

		m_deviceContext->IASetInputLayout(inputLayout);
		m_deviceContext->VSSetConstantBuffers(0, 1, &effect->GetVertexConstantBuffer());
		m_deviceContext->VSSetShader(effect->GetVertexShader(), nullptr, 0);
		m_deviceContext->PSSetConstantBuffers(0, 1, &effect->GetPixelConstantBuffer());
		m_deviceContext->PSSetShader(effect->GetPixelShader(), nullptr, 0);
		m_deviceContext->PSSetShaderResources(0, MAX_PIXEL_SHADER_RESOURCE_SLOTS, textureViews);
		m_deviceContext->PSSetSamplers(0, MAX_PIXEL_SHADER_RESOURCE_SLOTS, samplerStates);

		{
			auto blendState = GetBlendState(material->GetAlphaBlendingMode());
			m_deviceContext->OMSetBlendState(blendState, nullptr, ~0);
		}

		{
			DEPTHSTENCIL_STATE_INFO depthStencilStateInfo = {};
			depthStencilStateInfo.stencilTestEnabled	= material->GetStencilEnabled();
			depthStencilStateInfo.stencilFunction		= material->GetStencilFunction();
			depthStencilStateInfo.stencilFailAction		= material->GetStencilFailAction();
			depthStencilStateInfo.depthWriteEnabled		= (material->GetAlphaBlendingMode() == ALPHA_BLENDING_NONE) && !mesh->GetIsPeggedToOrigin();
			auto depthStencilState = GetDepthStencilState(depthStencilStateInfo);
			m_deviceContext->OMSetDepthStencilState(depthStencilState, material->GetStencilValue());
		}

		{
			RASTERIZER_STATE_INFO rasterizerStateInfo = {};
			rasterizerStateInfo.cullMode = material->GetCullingMode();
			auto rasterizerState = GetRasterizerState(rasterizerStateInfo);
			m_deviceContext->RSSetState(rasterizerState);
		}
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
