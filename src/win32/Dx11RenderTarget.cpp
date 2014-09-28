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

	HRESULT result = device->CreateShaderResourceView(m_texture, nullptr, &m_shaderResourceView);
	assert(SUCCEEDED(result));

	result = device->CreateRenderTargetView(m_texture, nullptr, &m_renderTargetView);
	assert(SUCCEEDED(result));
}

CDx11RenderTarget::~CDx11RenderTarget()
{

}

void CDx11RenderTarget::Draw(const ViewportPtr& viewport)
{
	auto& graphicDevice = static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance());
	static const float clearColor[4] = { 0, 0, 0, 0 };
	graphicDevice.GetDeviceContext()->ClearRenderTargetView(m_renderTargetView, clearColor);
	graphicDevice.DrawViewportMainMap(viewport.get(), m_renderTargetView, nullptr, m_width, m_height);
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
