#include "PlatformEffect.h"
#ifdef _WIN32
#include "Dx11ShaderGenerator.h"
#include "palleon/win32/Dx11GraphicDevice.h"
#elif defined(__ANDROID__) || defined(__APPLE__)
#include "GlEsShaderGenerator.h"
#endif

using namespace Palleon;

const char* CPlatformEffect::g_worldMatrixName			= "c_worldMatrix";
const char* CPlatformEffect::g_viewProjMatrixName		= "c_viewProjMatrix";
const char* CPlatformEffect::g_worldViewProjMatrixName	= "c_worldViewProjMatrix";
const char* CPlatformEffect::g_texture2MatrixName		= "c_texture2Matrix";
const char* CPlatformEffect::g_texture3MatrixName		= "c_texture3Matrix";

CPlatformEffect::CPlatformEffect(const CShaderBuilder& vertexShader, const CShaderBuilder& pixelShader)
#ifdef _WIN32
: CDx11Effect(
		static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).GetDevice(),
		static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).GetDeviceContext()
	)
#endif
{
	//On Dx11
	//Compile Shaders, Builds Constant Buffer, Creates Input Layout
	
	//On GlEs
	//Compile and Link program, bind uniform locations

#ifdef _WIN32
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
#elif defined(__ANDROID__) || defined(__APPLE__)
	AttributeBindingArray attributeBindings;
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_POSITION, "a_position0"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_UV0, "a_texCoord0"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_UV1, "a_texCoord1"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_COLOR, "a_color"));
	
	auto vertexShaderSource = CGlEsShaderGenerator::Generate(vertexShader, CGlEsShaderGenerator::SHADER_TYPE_VERTEX);
	auto pixelShaderSource = CGlEsShaderGenerator::Generate(pixelShader, CGlEsShaderGenerator::SHADER_TYPE_FRAGMENT);

	Palleon::CLog::GetInstance().Print("PlatformEffect: Vertex Shader Source: %s", vertexShaderSource.c_str());
	Palleon::CLog::GetInstance().Print("PlatformEffect: Pixel Shader Source: %s", pixelShaderSource.c_str());

	BuildProgram(vertexShaderSource, pixelShaderSource, attributeBindings);
	
	for(const auto& symbol : vertexShader.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		auto uniformName = vertexShader.GetUniformName(symbol);
		auto location = glGetUniformLocation(m_program, uniformName.c_str());
		//TODO: Look for error value
		assert(location != -1);
		m_vertexUniformLocations[uniformName] = location;
	}
	
	m_sampler0Location = glGetUniformLocation(m_program, "c_sampler0");
	m_sampler1Location = glGetUniformLocation(m_program, "c_sampler1");
	m_sampler2Location = glGetUniformLocation(m_program, "c_sampler2");
	m_sampler3Location = glGetUniformLocation(m_program, "c_sampler3");
	
#else
	assert(0);
#endif
}

CPlatformEffect::~CPlatformEffect()
{

}

void CPlatformEffect::UpdateConstants(const Palleon::VIEWPORT_PARAMS& viewportParams, Palleon::CMaterial* material, const CMatrix4& worldMatrix)
{
	BeginConstantsUpdate();
	UpdateConstantsInner(viewportParams, material, worldMatrix);
	EndConstantsUpdate();
}

void CPlatformEffect::UpdateConstantsInner(const Palleon::VIEWPORT_PARAMS& viewportParams, Palleon::CMaterial* material, const CMatrix4& worldMatrix)
{
	auto viewProjMatrix = viewportParams.viewMatrix * viewportParams.projMatrix;
	auto worldViewProjMatrix = worldMatrix * viewProjMatrix;

	SetConstant(g_worldMatrixName, worldMatrix);
	SetConstant(g_viewProjMatrixName, viewProjMatrix);
	SetConstant(g_worldViewProjMatrixName, worldViewProjMatrix);
	SetConstant(g_texture2MatrixName, material->GetTextureMatrix(2));
	SetConstant(g_texture3MatrixName, material->GetTextureMatrix(3));
}

#ifdef _WIN32

void CPlatformEffect::SetConstant(const std::string& name, const CMatrix4& matrix)
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

void CPlatformEffect::SetConstant(const std::string& name, const CEffectParameter& param)
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

void CPlatformEffect::BeginConstantsUpdate()
{
	assert(m_constantBufferPtr == nullptr);
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));
	m_constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
}

void CPlatformEffect::EndConstantsUpdate()
{
	assert(m_constantBufferPtr != nullptr);
	m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
	m_constantBufferPtr = nullptr;
}

Palleon::CPlatformEffect::D3D11InputLayoutPtr CPlatformEffect::CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR& descriptor)
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

#elif defined(__ANDROID__) || defined(__APPLE__)

void CPlatformEffect::BeginConstantsUpdate()
{
	glUniform1i(m_sampler0Location, 0);
	glUniform1i(m_sampler1Location, 1);
	glUniform1i(m_sampler2Location, 2);
	glUniform1i(m_sampler3Location, 3);
}

void CPlatformEffect::EndConstantsUpdate()
{

}

void CPlatformEffect::SetConstant(const std::string& name, const CMatrix4& matrix)
{
	auto constantLocationIterator = m_vertexUniformLocations.find(name);
	if(constantLocationIterator == std::end(m_vertexUniformLocations))
	{
		return;
	}

	auto constantLocation = constantLocationIterator->second;
	glUniformMatrix4fv(constantLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&matrix));
}

void CPlatformEffect::SetConstant(const std::string& name, const CEffectParameter& param)
{
	auto constantLocationIterator = m_vertexUniformLocations.find(name);
	if(constantLocationIterator == std::end(m_vertexUniformLocations))
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

	auto constantLocation = constantLocationIterator->second;
	glUniform4f(constantLocation, value.x, value.y, value.z, value.w);
}

#endif
