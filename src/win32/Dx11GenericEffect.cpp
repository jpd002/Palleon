#include "palleon/win32/Dx11GenericEffect.h"
#include "palleon/win32/Dx11ShaderGenerator.h"
#include "palleon/win32/Dx11GraphicDevice.h"
#include "palleon/graphics/OffsetKeeper.h"

using namespace Palleon;

CDx11GenericEffect::CDx11GenericEffect(const EffectInputBindingArray& inputBindings, const CShaderBuilder& vertexShader, const CShaderBuilder& pixelShader)
: CDx11Effect(
		static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).GetDevice(),
		static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).GetDeviceContext()
	)
, m_inputBindings(inputBindings)
{
	{
		auto vertexShaderCode = CDx11ShaderGenerator::Generate("VertexProgram", vertexShader);
		CompileVertexShader(vertexShaderCode);

		OffsetKeeper constantOffset;

		for(const auto& symbol : vertexShader.GetSymbols())
		{
			if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
			auto uniformName = vertexShader.GetUniformName(symbol);
			assert(IsVertexShaderUniform(uniformName.c_str()));
			uint32 uniformSize = 0;
			switch(symbol.type)
			{
			case CShaderBuilder::SYMBOL_TYPE_FLOAT:
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

		if(constantOffset.currentOffset != 0)
		{
			CreateVertexConstantBuffer(constantOffset.currentOffset);
		}
	}

	{
		auto pixelShaderCode = CDx11ShaderGenerator::Generate("PixelProgram", pixelShader);
		CompilePixelShader(pixelShaderCode);

		OffsetKeeper constantOffset;

		for(const auto& symbol : pixelShader.GetSymbols())
		{
			if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
			auto uniformName = pixelShader.GetUniformName(symbol);
			assert(IsPixelShaderUniform(uniformName.c_str()));
			uint32 uniformSize = 0;
			switch(symbol.type)
			{
			case CShaderBuilder::SYMBOL_TYPE_FLOAT:
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
			m_pixelUniformOffsets[uniformName] = offset;
		}

		if(constantOffset.currentOffset != 0)
		{
			CreatePixelConstantBuffer(constantOffset.currentOffset);
		}
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
	uint8* constantBufferPtr = nullptr;
	const UniformOffsetMap* uniformOffsets = nullptr;

	if(IsVertexShaderUniform(name.c_str()))
	{
		constantBufferPtr = m_vertexConstantBufferPtr;
		uniformOffsets = &m_vertexUniformOffsets;
	}
	else if(IsPixelShaderUniform(name.c_str()))
	{
		constantBufferPtr = m_pixelConstantBufferPtr;
		uniformOffsets = &m_pixelUniformOffsets;
	}

	assert(uniformOffsets != nullptr);

	auto constantOffsetIterator = uniformOffsets->find(name);
	if(constantOffsetIterator == std::end(*uniformOffsets))
	{
		return;
	}
	
	auto constantOffset = constantOffsetIterator->second;

	assert(constantBufferPtr != nullptr);
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + constantOffset) = matrix;
}

void CDx11GenericEffect::SetConstant(const std::string& name, const CEffectParameter& param)
{
	uint8* constantBufferPtr = nullptr;
	const UniformOffsetMap* uniformOffsets = nullptr;

	if(IsVertexShaderUniform(name.c_str()))
	{
		constantBufferPtr = m_vertexConstantBufferPtr;
		uniformOffsets = &m_vertexUniformOffsets;
	}
	else if(IsPixelShaderUniform(name.c_str()))
	{
		constantBufferPtr = m_pixelConstantBufferPtr;
		uniformOffsets = &m_pixelUniformOffsets;
	}

	assert(uniformOffsets != nullptr);

	auto constantOffsetIterator = uniformOffsets->find(name);
	if(constantOffsetIterator == std::end(*uniformOffsets))
	{
		return;
	}

	assert(constantBufferPtr != nullptr);

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
	*reinterpret_cast<CVector4*>(constantBufferPtr + constantOffset) = value;
}

void CDx11GenericEffect::BeginConstantsUpdate()
{
	if(!m_vertexConstantBuffer.IsEmpty())
	{
		assert(m_vertexConstantBufferPtr == nullptr);
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		m_vertexConstantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	}
	if(!m_pixelConstantBuffer.IsEmpty())
	{
		assert(m_pixelConstantBufferPtr == nullptr);
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_pixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		m_pixelConstantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	}
}

void CDx11GenericEffect::EndConstantsUpdate()
{
	if(!m_vertexConstantBuffer.IsEmpty())
	{
		assert(m_vertexConstantBufferPtr != nullptr);
		m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
		m_vertexConstantBufferPtr = nullptr;
	}
	if(!m_pixelConstantBuffer.IsEmpty())
	{
		assert(m_pixelConstantBufferPtr != nullptr);
		m_deviceContext->Unmap(m_pixelConstantBuffer, 0);
		m_pixelConstantBufferPtr = nullptr;
	}
}

Palleon::CDx11Effect::D3D11InputLayoutPtr CDx11GenericEffect::CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

	InputElementArray inputElements;

	for(const auto& inputBinding : m_inputBindings)
	{
		const auto& vertexItem = descriptor.GetVertexItem(inputBinding.vertexItemId);
		assert(vertexItem != nullptr);

		const char* semanticName = nullptr;
		switch(inputBinding.semantic)
		{
		case SEMANTIC_POSITION:
			semanticName = "POSITION";
			break;
		case SEMANTIC_TEXCOORD:
			semanticName = "TEXCOORD";
			break;
		default:
			assert(false);
			break;
		}

		DXGI_FORMAT elementFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		switch(vertexItem->size)
		{
		case 0x08:
			elementFormat = DXGI_FORMAT_R32G32_FLOAT;
			break;
		case 0x0C:
			elementFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		default:
			assert(false);
			break;
		}

		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= semanticName;
		inputElement.SemanticIndex			= inputBinding.semanticIndex;
		inputElement.Format					= elementFormat;
		inputElement.AlignedByteOffset		= vertexItem->offset;
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
