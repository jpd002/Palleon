#include "athena/gles/GlEsUberEffect.h"
#include "athena/VertexBuffer.h"

using namespace Athena;

CGlEsUberEffect::CGlEsUberEffect(const EFFECTCAPS& effectCaps)
{
	memset(m_diffuseTextureHandle, -1, sizeof(m_diffuseTextureHandle));
	memset(m_diffuseTextureMatrixHandle, -1, sizeof(m_diffuseTextureMatrixHandle));
	
	auto vertexShaderSource = CGlEsUberEffectGenerator::GenerateVertexShader(effectCaps);
	auto pixelShaderSource = CGlEsUberEffectGenerator::GeneratePixelShader(effectCaps);
		
	AttributeBindingArray attributeBindings;
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_POSITION, "a_position"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_UV0, "a_texCoord0"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_UV1, "a_texCoord1"));
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_COLOR, "a_color"));
	
	BuildProgram(vertexShaderSource, pixelShaderSource, attributeBindings);
	
	m_viewProjMatrixHandle			= glGetUniformLocation(m_program, "c_viewProjMatrix");
	m_worldMatrixHandle				= glGetUniformLocation(m_program, "c_worldMatrix");
	m_meshColorHandle				= glGetUniformLocation(m_program, "c_meshColor");
	m_shadowViewProjMatrixHandle	= glGetUniformLocation(m_program, "c_shadowViewProjMatrix");
	
	for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
	{
		{
			char paramName[256];
			sprintf(paramName, "c_diffuseTexture%d", i);
			m_diffuseTextureHandle[i]	= glGetUniformLocation(m_program, paramName);
		}
		
		{
			char paramName[256];
			sprintf(paramName, "c_diffuseTextureMatrix%d", i);
			m_diffuseTextureMatrixHandle[i] = glGetUniformLocation(m_program, paramName);
		}
	}
	
	m_shadowTextureHandle = glGetUniformLocation(m_program, "c_shadowTexture");
}

CGlEsUberEffect::~CGlEsUberEffect()
{
	
}

void CGlEsUberEffect::UpdateConstants(const MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewMatrix, const CMatrix4& projMatrix, const CMatrix4& shadowViewProjMatrix)
{
	auto meshColor = material->GetColor();
	auto viewProjMatrix = viewMatrix * projMatrix;

	glUniformMatrix4fv(m_viewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&viewProjMatrix));
	glUniformMatrix4fv(m_worldMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&worldMatrix));
	glUniform4f(m_meshColorHandle, meshColor.r, meshColor.g, meshColor.b, meshColor.a);
	
	if(m_shadowViewProjMatrixHandle != -1)
	{
		glUniformMatrix4fv(m_shadowViewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&shadowViewProjMatrix));
	}
	
	unsigned int textureCount = 0;
	for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
	{
		if(m_diffuseTextureHandle[i] != -1)
		{
			const auto& textureMatrix(material->GetTextureMatrix(i));
			glUniform1i(m_diffuseTextureHandle[i], i);
			glUniformMatrix4fv(m_diffuseTextureMatrixHandle[i], 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&textureMatrix));
			textureCount++;
		}
	}
	
	if(m_shadowTextureHandle != -1)
	{
		glUniform1i(m_shadowTextureHandle, textureCount);
		textureCount++;
	}
}
