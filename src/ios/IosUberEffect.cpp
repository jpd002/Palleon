#include "athena/ios/IosUberEffect.h"

using namespace Athena;

CIosUberEffect::CIosUberEffect(const EFFECTCAPS& effectCaps)
{
	memset(m_diffuseTextureHandle, -1, sizeof(m_diffuseTextureHandle));
	memset(m_diffuseTextureMatrixHandle, -1, sizeof(m_diffuseTextureMatrixHandle));
	
	auto vertexShaderSource = CIosUberEffectGenerator::GenerateVertexShader(effectCaps);
	auto pixelShaderSource = CIosUberEffectGenerator::GeneratePixelShader(effectCaps);
	
	BuildProgram(vertexShaderSource, pixelShaderSource);
	
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

CIosUberEffect::~CIosUberEffect()
{
	
}

void CIosUberEffect::UpdateConstants(const MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewProjMatrix, const CMatrix4& shadowViewProjMatrix)
{
	auto meshColor = material->GetColor();
	
	//if(mesh->GetIsPeggedToOrigin())
	//{
	//	glUniformMatrix4fv(currentEffect->viewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&m_peggedViewProjMatrix));
	//}
	//else
	{
		glUniformMatrix4fv(m_viewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&viewProjMatrix));
	}
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
