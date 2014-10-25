#include "palleon/win32/Dx11GenericEffect.h"
#include "palleon/win32/Dx11ShaderGenerator.h"
#include "palleon/win32/Dx11GraphicDevice.h"

using namespace Palleon;

CDx11GenericEffect::CDx11GenericEffect(CShaderBuilder& vertexShader, CShaderBuilder& pixelShader)
: CDx11Effect(
		static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).GetDevice(),
		static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).GetDeviceContext()
	)
{
	{
		auto vertexShaderCode = CDx11ShaderGenerator::Generate("VertexProgram", vertexShader);
		CompileVertexShader(vertexShaderCode);

		OffsetKeeper constantOffset;

		for(const auto& symbol : vertexShader.GetSymbols())
		{
			if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
			auto uniformName = vertexShader.GetUniformName(symbol);
			uint32 uniformSize = 0;
			switch(symbol.type)
			{
			case CShaderBuilder::SYMBOL_TYPE_FLOAT3:
				uniformSize = sizeof(CVector4);
				break;
			case CShaderBuilder::SYMBOL_TYPE_MATRIX:
				uniformSize = sizeof(CMatrix4);
				break;
			default:
				assert(0);
				break;
			}
			uint32 offset = constantOffset.Allocate(uniformSize);
			m_vertexUniformOffsets[uniformName] = offset;
		}

		CreateVertexConstantBuffer(constantOffset.currentOffset);
	}

	{
		auto pixelShaderCode = CDx11ShaderGenerator::Generate("PixelProgram", pixelShader);
		CompilePixelShader(pixelShaderCode);
	}
}

CDx11GenericEffect::~CDx11GenericEffect()
{

}

void CDx11GenericEffect::UpdateConstants(const VIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	BeginConstantsUpdate();
	UpdateGenericConstants(viewportParams, material, worldMatrix);
	UpdateSpecificConstants(viewportParams, material, worldMatrix);
	EndConstantsUpdate();
}

void CDx11GenericEffect::SetConstant(const std::string& name, const CMatrix4& matrix)
{
	assert(m_constantBufferPtr != nullptr);

	auto constantOffsetIterator = m_vertexUniformOffsets.find(name);
	if(constantOffsetIterator == std::end(m_vertexUniformOffsets))
	{
		return;
	}

	auto constantOffset = constantOffsetIterator->second;
	*reinterpret_cast<CMatrix4*>(m_constantBufferPtr + constantOffset) = matrix;
}

void CDx11GenericEffect::SetConstant(const std::string& name, const CEffectParameter& param)
{
	assert(m_constantBufferPtr != nullptr);

	auto constantOffsetIterator = m_vertexUniformOffsets.find(name);
	if(constantOffsetIterator == std::end(m_vertexUniformOffsets))
	{
		return;
	}

	CVector4 value(0, 0, 0, 0);
	if(param.IsScalar())
	{
		value.x = param.GetScalar();
	}
	else if(param.IsVector3())
	{
		value = CVector4(param.GetVector3(), 0);
	}
	else
	{
		assert(0);
	}

	auto constantOffset = constantOffsetIterator->second;
	*reinterpret_cast<CVector4*>(m_constantBufferPtr + constantOffset) = value;
}

void CDx11GenericEffect::BeginConstantsUpdate()
{
	assert(m_constantBufferPtr == nullptr);
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));
	m_constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
}

void CDx11GenericEffect::EndConstantsUpdate()
{
	assert(m_constantBufferPtr != nullptr);
	m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
	m_constantBufferPtr = nullptr;
}

Palleon::CDx11Effect::D3D11InputLayoutPtr CDx11GenericEffect::CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	const auto& posVertexItem = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_POSITION);
	const auto& texCoordItem = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_UV0);
	assert(posVertexItem != nullptr);
	assert(texCoordItem != nullptr);

	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

	InputElementArray inputElements;

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "POSITION";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= posVertexItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= texCoordItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	D3D11InputLayoutPtr inputLayout;
	HRESULT result = m_device->CreateInputLayout(inputElements.data(), inputElements.size(), 
		m_vertexShaderCode->GetBufferPointer(), m_vertexShaderCode->GetBufferSize(), &inputLayout);
	assert(SUCCEEDED(result));

	return inputLayout;
}
