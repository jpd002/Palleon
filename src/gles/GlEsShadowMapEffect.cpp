#include "palleon/gles/GlEsShadowMapEffect.h"
#include "palleon/graphics/VertexBuffer.h"

using namespace Palleon;

CGlEsShadowMapEffect::CGlEsShadowMapEffect()
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
	
	AttributeBindingArray attributeBindings;
	attributeBindings.push_back(std::make_pair(VERTEX_ITEM_ID_POSITION, "a_position"));
	
	BuildProgram(g_vertexShader, g_pixelShader, attributeBindings);
	
	m_viewProjMatrixHandle	= glGetUniformLocation(m_program, "c_viewProjMatrix");
	m_worldMatrixHandle		= glGetUniformLocation(m_program, "c_worldMatrix");
}

CGlEsShadowMapEffect::~CGlEsShadowMapEffect()
{
	
}

void CGlEsShadowMapEffect::UpdateConstants(const VIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	auto viewProjMatrix = viewportParams.viewMatrix * viewportParams.projMatrix;
	glUniformMatrix4fv(m_viewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&viewProjMatrix));
	glUniformMatrix4fv(m_worldMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&worldMatrix));
}
