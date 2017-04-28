#pragma once

#define NOMINMAX
#include <d3d11.h>
#include <memory>
#include <string>
#include "Types.h"
#include "win32/ComPtr.h"
#include "palleon/graphics/VertexBuffer.h"
#include "palleon/graphics/Effect.h"

namespace Palleon
{
	class CDx11Effect;
	typedef std::shared_ptr<CDx11Effect> Dx11EffectPtr;

	class CDx11Effect : public CEffect
	{
	public:
		typedef Framework::Win32::CComPtr<ID3D11VertexShader> D3D11VertexShaderPtr;
		typedef Framework::Win32::CComPtr<ID3D11PixelShader> D3D11PixelShaderPtr;
		typedef Framework::Win32::CComPtr<ID3DBlob> D3DBlobPtr;
		typedef Framework::Win32::CComPtr<ID3D11Buffer> D3D11BufferPtr;
		typedef Framework::Win32::CComPtr<ID3D11InputLayout> D3D11InputLayoutPtr;

								CDx11Effect(ID3D11Device*, ID3D11DeviceContext*);
		virtual					~CDx11Effect();

		D3D11VertexShaderPtr	GetVertexShader() const;
		D3D11PixelShaderPtr		GetPixelShader() const;
		D3D11BufferPtr			GetVertexConstantBuffer() const;
		D3D11BufferPtr			GetPixelConstantBuffer() const;
		D3D11InputLayoutPtr		GetInputLayout(const VERTEX_BUFFER_DESCRIPTOR&);

	protected:
		typedef std::unordered_map<Palleon::VERTEX_ITEMS_KEY, D3D11InputLayoutPtr> InputLayoutMap;

		void							CompileVertexShader(const std::string&);
		void							CompilePixelShader(const std::string&);
		void							CreateVertexConstantBuffer(uint32);
		void							CreatePixelConstantBuffer(uint32);
		virtual D3D11InputLayoutPtr		CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR&) = 0;

		ID3D11Device*					m_device;
		ID3D11DeviceContext*			m_deviceContext;
		D3D11VertexShaderPtr			m_vertexShader;
		D3DBlobPtr						m_vertexShaderCode;
		D3D11PixelShaderPtr				m_pixelShader;
		D3D11BufferPtr					m_vertexConstantBuffer;
		D3D11BufferPtr					m_pixelConstantBuffer;
		InputLayoutMap					m_inputLayouts;
	};
}
