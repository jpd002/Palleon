#include "athena/ios/IosShadowMapEffect.h"

using namespace Athena;

CIosShadowMapEffect::CIosShadowMapEffect()
{
	static const char* g_vertexShader =
		"attribute vec3 a_position;"
		"uniform mat4 c_worldMatrix;"
		"uniform mat4 c_viewProjMatrix;"
		"varying vec4 v_position;"
		"void main()"
		"{"
		"vec4 worldPosition = c_worldMatrix * vec4(a_position, 1);"
		"v_position = c_viewProjMatrix * worldPosition;"
		"gl_Position = v_position;"
		"}";
	
	static const char* g_pixelShader =
		"varying highp vec4 v_position;"
		"void main()"
		"{"
		"gl_FragColor = vec4(v_position.z / v_position.w, 1, 1, 1);"
		"}";
	
	BuildProgram(g_vertexShader, g_pixelShader);
	
	m_viewProjMatrixHandle	= glGetUniformLocation(m_program, "c_viewProjMatrix");
	m_worldMatrixHandle		= glGetUniformLocation(m_program, "c_worldMatrix");
}

CIosShadowMapEffect::~CIosShadowMapEffect()
{
	
}

void CIosShadowMapEffect::UpdateConstants(const MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewProjMatrix, const CMatrix4& shadowViewProjMatrix)
{
	glUniformMatrix4fv(m_viewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&viewProjMatrix));
	glUniformMatrix4fv(m_worldMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&worldMatrix));
}
