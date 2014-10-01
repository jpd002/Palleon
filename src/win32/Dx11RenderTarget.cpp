#include <stdexcept>
#include "palleon/win32/Dx11RenderTarget.h"
#include "palleon/win32/Dx11GraphicDevice.h"

using namespace Palleon;

CDx11RenderTarget::CDx11RenderTarget(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
: m_width(width)
, m_height(height)
{
	auto specTextureFormat = CDx11GraphicDevice::g_textureFormats[textureFormat];

	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= width;
		textureDesc.Height				= height;
		textureDesc.MipLevels			= 1;
		textureDesc.ArraySize			= 1;
		textureDesc.Format				= specTextureFormat;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.Usage				= D3D11_USAGE_DEFAULT;
		textureDesc.SampleDesc.Count	= 1;
		textureDesc.SampleDesc.Quality	= 0;
		HRESULT result = device->CreateTexture2D(&textureDesc, nullptr, &m_texture);
		assert(SUCCEEDED(result));
	}

	{
		D3D11_TEXTURE2D_DESC depthBufferDesc = {};

		depthBufferDesc.Width				= width;
		depthBufferDesc.Height				= height;
		depthBufferDesc.MipLevels			= 1;
		depthBufferDesc.ArraySize			= 1;
		depthBufferDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.SampleDesc.Count	= 1;
		depthBufferDesc.SampleDesc.Quality	= 0;

		HRESULT result = device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthBuffer);
		assert(SUCCEEDED(result));
	}

	HRESULT result = device->CreateShaderResourceView(m_texture, nullptr, &m_shaderResourceView);
	assert(SUCCEEDED(result));

	result = device->CreateRenderTargetView(m_texture, nullptr, &m_renderTargetView);
	assert(SUCCEEDED(result));

	result = device->CreateDepthStencilView(m_depthBuffer, nullptr, &m_depthBufferView);
	assert(SUCCEEDED(result));
}

CDx11RenderTarget::~CDx11RenderTarget()
{

}

void CDx11RenderTarget::Clear()
{
	auto& graphicDevice = static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance());
	static const float clearColor[4] = { 0, 0, 0, 0 };
	graphicDevice.GetDeviceContext()->ClearRenderTargetView(m_renderTargetView, clearColor);
	graphicDevice.GetDeviceContext()->ClearDepthStencilView(m_depthBufferView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CDx11RenderTarget::Draw(const ViewportPtr& viewport)
{
	auto& graphicDevice = static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance());
	graphicDevice.DrawViewportMainMap(viewport.get(), m_renderTargetView, m_depthBufferView, m_width, m_height);
}

void CDx11RenderTarget::Update(uint32, const void*)
{
	throw std::runtime_error("Not supported.");
}

void CDx11RenderTarget::UpdateCubeFace(TEXTURE_CUBE_FACE, const void*)
{
	throw std::runtime_error("Not supported.");
}

void* CDx11RenderTarget::GetHandle() const
{
	return m_shaderResourceView;
}
