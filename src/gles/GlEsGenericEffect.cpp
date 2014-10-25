#include "palleon/gles/GlEsGenericEffect.h"
#include "palleon/gles/GlEsShaderGenerator.h"
#include "palleon/gles/GlEsGraphicDevice.h"

using namespace Palleon;

CGlEsGenericEffect::CGlEsGenericEffect(const CShaderBuilder& vertexShader, const CShaderBuilder& pixelShader)
{
	AttributeBindingArray attributeBindings;
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_POSITION, "a_position0"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_UV0, "a_texCoord0"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_UV1, "a_texCoord1"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_COLOR, "a_color"));
	
	auto vertexShaderSource = CGlEsShaderGenerator::Generate(vertexShader, CGlEsShaderGenerator::SHADER_TYPE_VERTEX);
	auto pixelShaderSource = CGlEsShaderGenerator::Generate(pixelShader, CGlEsShaderGenerator::SHADER_TYPE_FRAGMENT);

//	Palleon::CLog::GetInstance().Print("PlatformEffect: Vertex Shader Source: %s", vertexShaderSource.c_str());
//	Palleon::CLog::GetInstance().Print("PlatformEffect: Pixel Shader Source: %s", pixelShaderSource.c_str());

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
}

CGlEsGenericEffect::~CGlEsGenericEffect()
{

}

void CGlEsGenericEffect::UpdateConstants(const VIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	glUniform1i(m_sampler0Location, 0);
	glUniform1i(m_sampler1Location, 1);
	glUniform1i(m_sampler2Location, 2);
	glUniform1i(m_sampler3Location, 3);

	UpdateGenericConstants(viewportParams, material, worldMatrix);
	UpdateSpecificConstants(viewportParams, material, worldMatrix);
}

void CGlEsGenericEffect::SetConstant(const std::string& name, const CMatrix4& matrix)
{
	auto constantLocationIterator = m_vertexUniformLocations.find(name);
	if(constantLocationIterator == std::end(m_vertexUniformLocations))
	{
		return;
	}

	auto constantLocation = constantLocationIterator->second;
	glUniformMatrix4fv(constantLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&matrix));
}

void CGlEsGenericEffect::SetConstant(const std::string& name, const CEffectParameter& param)
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
