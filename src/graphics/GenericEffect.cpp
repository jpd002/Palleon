#include "palleon/graphics/GenericEffect.h"

using namespace Palleon;

const char* CGenericEffect::g_worldMatrixName			= "v_worldMatrix";
const char* CGenericEffect::g_viewProjMatrixName		= "v_viewProjMatrix";
const char* CGenericEffect::g_worldViewProjMatrixName	= "v_worldViewProjMatrix";
const char* CGenericEffect::g_texture2MatrixName		= "v_texture2Matrix";
const char* CGenericEffect::g_texture3MatrixName		= "v_texture3Matrix";

bool CGenericEffect::IsVertexShaderUniform(const char* uniformName)
{
	return strstr(uniformName, "v_") == uniformName;
}

bool CGenericEffect::IsPixelShaderUniform(const char* uniformName)
{
	return strstr(uniformName, "p_") == uniformName;
}

void CGenericEffect::UpdateGenericConstants(const Palleon::VIEWPORT_PARAMS& viewportParams, Palleon::CMaterial* material, const CMatrix4& worldMatrix)
{
	auto viewProjMatrix = viewportParams.viewMatrix * viewportParams.projMatrix;
	auto worldViewProjMatrix = worldMatrix * viewProjMatrix;

	SetConstant(g_worldMatrixName, worldMatrix);
	SetConstant(g_viewProjMatrixName, viewProjMatrix);
	SetConstant(g_worldViewProjMatrixName, worldViewProjMatrix);
	SetConstant(g_texture2MatrixName, material->GetTextureMatrix(2));
	SetConstant(g_texture3MatrixName, material->GetTextureMatrix(3));
}
