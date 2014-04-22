#include "palleon/win32/Dx11UberEffectGenerator.h"
#include <stdarg.h>

using namespace Palleon;

std::string PrintLine(const char* format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 256, format, args);
	va_end(args);
	return std::string(buffer) + std::string("\r\n");
}

std::string CDx11UberEffectGenerator::GenerateVertexShader(const EFFECTCAPS& caps)
{
	bool needsTexCoord0 = HasCoordSrc(caps, DIFFUSE_MAP_COORD_UV0);
	bool needsTexCoord1 = HasCoordSrc(caps, DIFFUSE_MAP_COORD_UV1);

	std::string result;

	result += PrintLine("cbuffer MatrixBuffer");
	result += PrintLine("{");
	result += PrintLine("float4 c_meshColor;");
	result += PrintLine("matrix c_worldMatrix;");
	result += PrintLine("matrix c_viewProjMatrix;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapMatrix(0);
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapMatrix(1);
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapMatrix(2);
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapMatrix(3);
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapMatrix(4);
	if(caps.hasShadowMap) result += PrintLine("matrix c_shadowViewProjMatrix;");
	result += PrintLine("};");

	result += PrintLine("struct VertexInputType");
	result += PrintLine("{");
	result += PrintLine("float3 position : POSITION;");
	if(caps.hasVertexColor)
	{
		result += PrintLine("float4 color : COLOR;");
	}
	if(needsTexCoord0)
	{
		result += PrintLine("float2 texCoord0 : TEXCOORD0;");
	}
	if(needsTexCoord1)
	{
		result += PrintLine("float2 texCoord1 : TEXCOORD1;");
	}
	result += PrintLine("};");

	result += PrintLine("struct PixelInputType");
	result += PrintLine("{");
	result += PrintLine("float4 position : SV_POSITION;");
	result += PrintLine("float4 color : COLOR;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordOutput(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapCoordOutput(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapCoordOutput(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapCoordOutput(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapCoordOutput(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));
	if(caps.hasShadowMap) result += PrintLine("float4 shadowPosition : POSITION;");
	result += PrintLine("};");

	result += PrintLine("PixelInputType VertexProgram(VertexInputType input)");
	result += PrintLine("{");
	result += PrintLine("PixelInputType output;");
	result += PrintLine("float4 worldPosition = mul(c_worldMatrix, float4(input.position, 1));");
	result += PrintLine("output.position = mul(c_viewProjMatrix, worldPosition);");
	if(caps.hasVertexColor)
	{
		result += PrintLine("output.color = input.color * c_meshColor;");
	}
	else
	{
		result += PrintLine("output.color = c_meshColor;");
	}

	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordComputation(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapCoordComputation(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapCoordComputation(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapCoordComputation(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapCoordComputation(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));

	if(caps.hasShadowMap) result += PrintLine("output.shadowPosition = mul(c_shadowViewProjMatrix, worldPosition);");

	result += PrintLine("return output;");
	result += PrintLine("}");

	return result;
}

std::string CDx11UberEffectGenerator::GeneratePixelShader(const EFFECTCAPS& caps)
{
	std::string result;

	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapSampler(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapSampler(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapSampler(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapSampler(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapSampler(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));
	if(caps.hasShadowMap)
	{
		result += PrintLine("Texture2D c_shadowTexture;");
		result += PrintLine("SamplerState c_shadowSampler;");
	}
	
	result += PrintLine("struct PixelInputType");
	result += PrintLine("{");
	result += PrintLine("float4 position : SV_POSITION;");
	result += PrintLine("float4 color : COLOR;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordOutput(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapCoordOutput(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapCoordOutput(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapCoordOutput(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapCoordOutput(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));
	if(caps.hasShadowMap) result += PrintLine("float4 shadowPosition : POSITION;");
	result += PrintLine("};");

	result += PrintLine("float4 PixelProgram(PixelInputType input) : SV_TARGET");
	result += PrintLine("{");
	result += PrintLine("	float4 diffuseColor = input.color;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapSampling(0, static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap0CombineMode));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapSampling(1, static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap1CombineMode));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapSampling(2, static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap2CombineMode));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapSampling(3, static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap3CombineMode));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapSampling(4, static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap4CombineMode));

	if(caps.hasShadowMap) 
	{
		result += PrintLine("float2 shadowMapCoord = 0.5 * (input.shadowPosition.xy / input.shadowPosition.w) + float2(0.5, 0.5);");
		result += PrintLine("shadowMapCoord.y = 1 - shadowMapCoord.y;");
		result += PrintLine("float pixelZ = input.shadowPosition.z / input.shadowPosition.w;");
		result += PrintLine("float shadowZ = c_shadowTexture.Sample(c_shadowSampler, shadowMapCoord).r;");
		result += PrintLine("float shadowBias = 0.03f;");
		result += PrintLine("if(shadowZ < pixelZ - shadowBias)");
		result += PrintLine("{");
		result += PrintLine("	diffuseColor.xyz *= 0.25f;");
		result += PrintLine("}");
	}
	
	result += PrintLine("	return diffuseColor;");
	result += PrintLine("}");

	return result;
}

bool CDx11UberEffectGenerator::HasCoordSrc(const EFFECTCAPS& caps, unsigned int texCoordSource)
{
	return 
		(caps.hasDiffuseMap0 && (caps.diffuseMap0CoordSrc == texCoordSource)) || 
		(caps.hasDiffuseMap1 && (caps.diffuseMap1CoordSrc == texCoordSource)) || 
		(caps.hasDiffuseMap2 && (caps.diffuseMap2CoordSrc == texCoordSource)) || 
		(caps.hasDiffuseMap3 && (caps.diffuseMap3CoordSrc == texCoordSource)) || 
		(caps.hasDiffuseMap4 && (caps.diffuseMap4CoordSrc == texCoordSource));
}

std::string CDx11UberEffectGenerator::GenerateDiffuseMapCoordOutput(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;
	switch(source)
	{
	case DIFFUSE_MAP_COORD_UV0:
	case DIFFUSE_MAP_COORD_UV1:
		result += PrintLine("float2		diffuseCoord%d : TEXCOORD%d;", index, index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_POS:
	case DIFFUSE_MAP_COORD_CUBE_REFLECT:
		result += PrintLine("float3		diffuseCoord%d : TEXCOORD%d;", index, index);
		break;
	}
	return result;
}

std::string CDx11UberEffectGenerator::GenerateDiffuseMapCoordComputation(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;

	switch(source)
	{
	case DIFFUSE_MAP_COORD_UV0:
		result += PrintLine("	output.diffuseCoord%d = mul(c_diffuse%dTextureMatrix, float4(input.texCoord0, 0, 1));", index, index);
		break;
	case DIFFUSE_MAP_COORD_UV1:
		result += PrintLine("	output.diffuseCoord%d = mul(c_diffuse%dTextureMatrix, float4(input.texCoord1, 0, 1));", index, index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_POS:
		result += PrintLine("	output.diffuseCoord%d = normalize(worldPosition).xyz;", index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_REFLECT:
		result += PrintLine("	output.diffuseCoord%d = reflect(viewDir, worldNrm);", index);
		break;
	}

	return result;
}

std::string CDx11UberEffectGenerator::GenerateDiffuseMapSampler(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;

	switch(source)
	{
	case DIFFUSE_MAP_COORD_UV0:
	case DIFFUSE_MAP_COORD_UV1:
		result += PrintLine("Texture2D c_diffuse%dTexture;", index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_POS:
	case DIFFUSE_MAP_COORD_CUBE_REFLECT:
		result += PrintLine("TextureCube c_diffuse%dTexture;", index);
		break;
	}

	result += PrintLine("SamplerState c_diffuse%dSampler;", index);

	return result;
}

std::string CDx11UberEffectGenerator::GenerateDiffuseMapMatrix(unsigned int index)
{
	std::string result;

	result += PrintLine("matrix c_diffuse%dTextureMatrix;", index);

	return result;
}

std::string CDx11UberEffectGenerator::GenerateDiffuseMapSampling(unsigned int index, DIFFUSE_MAP_COMBINE_MODE combineMode)
{
	std::string result;

	result += PrintLine("	float4 diffuseColor%d = c_diffuse%dTexture.Sample(c_diffuse%dSampler, input.diffuseCoord%d);", index, index, index, index);

	switch(combineMode)
	{
	case DIFFUSE_MAP_COMBINE_MODULATE:
		result += PrintLine("	diffuseColor *= diffuseColor%d;", index);
		break;
	case DIFFUSE_MAP_COMBINE_ADD:
		result += PrintLine("	diffuseColor += diffuseColor%d;", index);
		break;
	case DIFFUSE_MAP_COMBINE_LERP:
		result += PrintLine("	diffuseColor = lerp(diffuseColor, diffuseColor%d, diffuseColor%d.a);", index, index);
		break;
	}

	return result;
}
