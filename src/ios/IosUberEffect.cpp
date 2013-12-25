#include "athena/ios/IosUberEffect.h"

using namespace Athena;

CIosUberEffect::CIosUberEffect(const EFFECTCAPS& effectCaps)
{
	memset(m_diffuseTexture, -1, sizeof(m_diffuseTexture));
	memset(m_diffuseTextureMatrix, -1, sizeof(m_diffuseTextureMatrix));
	
	auto vertexShaderSource = CIosUberEffectGenerator::GenerateVertexShader(effectCaps);
	auto pixelShaderSource = CIosUberEffectGenerator::GeneratePixelShader(effectCaps);
	
	BuildProgram(vertexShaderSource, pixelShaderSource);
	
	m_viewProjMatrixHandle	= glGetUniformLocation(m_program, "c_viewProjMatrix");
	m_worldMatrixHandle		= glGetUniformLocation(m_program, "c_worldMatrix");
	m_meshColorHandle		= glGetUniformLocation(m_program, "c_meshColor");
	
	for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
	{
		{
			char paramName[256];
			sprintf(paramName, "c_diffuseTexture%d", i);
			m_diffuseTexture[i]	= glGetUniformLocation(m_program, paramName);
		}
		
		{
			char paramName[256];
			sprintf(paramName, "c_diffuseTextureMatrix%d", i);
			m_diffuseTextureMatrix[i] = glGetUniformLocation(m_program, paramName);
		}
	}
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
	
	for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
	{
		if(m_diffuseTexture[i] != -1)
		{
			const auto& textureMatrix(material->GetTextureMatrix(i));
			glUniform1i(m_diffuseTexture[i], i);
			glUniformMatrix4fv(m_diffuseTextureMatrix[i], 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&textureMatrix));
		}
	}
}
