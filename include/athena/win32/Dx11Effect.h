#pragma once

#include <d3d11.h>
#include <memory>
#include <string>
#include "Types.h"
#include "win32/ComPtr.h"
#include "athena/Matrix4.h"
#include "athena/Material.h"
#include "athena/VertexBuffer.h"
#include "athena/Effect.h"

namespace Athena
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
		D3DBlobPtr				GetVertexShaderCode() const;
		D3D11BufferPtr			GetConstantBuffer() const;
		D3D11InputLayoutPtr		GetInputLayout(const VERTEX_BUFFER_DESCRIPTOR&);

		virtual void			UpdateConstants(const MaterialPtr&, const CMatrix4& worldMatrix, const CMatrix4& viewProjMatrix, const CMatrix4& shadowViewProjMatrix) = 0;

	protected:
		struct OffsetKeeper
		{
			uint32 Allocate(uint32 size)
			{
				uint32 result = currentOffset;
				currentOffset += size;
				return result;
			}

			uint32 currentOffset = 0;
		};

		typedef std::unordered_map<uint64, D3D11InputLayoutPtr> InputLayoutMap;

		void					CompileVertexShader(const std::string&);
		void					CompilePixelShader(const std::string&);
		void					CreateConstantBuffer(uint32);
		D3D11InputLayoutPtr		CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR&);

		ID3D11Device*			m_device;
		ID3D11DeviceContext*	m_deviceContext;
		D3D11VertexShaderPtr	m_vertexShader;
		D3DBlobPtr				m_vertexShaderCode;
		D3D11PixelShaderPtr		m_pixelShader;
		D3D11BufferPtr			m_constantBuffer;

		InputLayoutMap			m_inputLayouts;
	};
}
