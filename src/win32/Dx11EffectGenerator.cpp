#include "athena/win32/Dx11EffectGenerator.h"
#include <stdarg.h>

using namespace Athena;

std::string PrintLine(const char* format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 256, format, args);
	va_end(args);
	return std::string(buffer) + std::string("\r\n");
}

std::string CDx11EffectGenerator::GenerateVertexShader(const EFFECTCAPS& caps)
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
	result += PrintLine("};");

	result += PrintLine("PixelInputType VertexProgram(VertexInputType input)");
	result += PrintLine("{");
	result += PrintLine("PixelInputType output;");
	result += PrintLine("output.position = mul(c_worldMatrix, float4(input.position, 1));");
	result += PrintLine("output.position = mul(c_viewProjMatrix, output.position);");
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

	result += PrintLine("return output;");
	result += PrintLine("}");

	return result;
}

std::string CDx11EffectGenerator::GeneratePixelShader(const EFFECTCAPS& caps)
{
	std::string result;

	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapSampler(0);
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapSampler(1);
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapSampler(2);
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapSampler(3);
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapSampler(4);

	result += PrintLine("struct PixelInputType");
	result += PrintLine("{");
	result += PrintLine("float4 position : SV_POSITION;");
	result += PrintLine("float4 color : COLOR;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordOutput(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapCoordOutput(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapCoordOutput(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapCoordOutput(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapCoordOutput(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));
	result += PrintLine("};");

	result += PrintLine("float4 PixelProgram(PixelInputType input) : SV_TARGET");
	result += PrintLine("{");
	result += PrintLine("	float4 diffuseColor = input.color;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapSampling(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap0CombineMode));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapSampling(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap1CombineMode));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapSampling(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap2CombineMode));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapSampling(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap3CombineMode));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapSampling(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap4CombineMode));
	result += PrintLine("	return diffuseColor;");
	result += PrintLine("}");

	return result;
}

bool CDx11EffectGenerator::HasCoordSrc(const EFFECTCAPS& caps, unsigned int texCoordSource)
{
	return 
		(caps.hasDiffuseMap0 && (caps.diffuseMap0CoordSrc == texCoordSource)) || 
		(caps.hasDiffuseMap1 && (caps.diffuseMap1CoordSrc == texCoordSource)) || 
		(caps.hasDiffuseMap2 && (caps.diffuseMap2CoordSrc == texCoordSource)) || 
		(caps.hasDiffuseMap3 && (caps.diffuseMap3CoordSrc == texCoordSource)) || 
		(caps.hasDiffuseMap4 && (caps.diffuseMap4CoordSrc == texCoordSource));
}

std::string CDx11EffectGenerator::GenerateDiffuseMapCoordOutput(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
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

std::string CDx11EffectGenerator::GenerateDiffuseMapCoordComputation(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;

	switch(source)
	{
	case DIFFUSE_MAP_COORD_UV0:
		result += PrintLine("	output.diffuseCoord%d = mul(float4(input.texCoord0, 0, 1), c_diffuse%dTextureMatrix);", index, index);
		break;
	case DIFFUSE_MAP_COORD_UV1:
		result += PrintLine("	output.diffuseCoord%d = mul(float4(input.texCoord1, 0, 1), c_diffuse%dTextureMatrix);", index, index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_POS:
		result += PrintLine("	output.diffuseCoord%d = normalize(worldPos).xyz;", index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_REFLECT:
		result += PrintLine("	output.diffuseCoord%d = reflect(viewDir, worldNrm);", index);
		break;
	}

	return result;
}

std::string CDx11EffectGenerator::GenerateDiffuseMapSampler(unsigned int index)
{
	std::string result;

	result += PrintLine("texture2D c_diffuse%dTexture;", index);
	result += PrintLine("SamplerState c_diffuse%dSampler;", index);

	return result;
}

std::string CDx11EffectGenerator::GenerateDiffuseMapMatrix(unsigned int index)
{
	std::string result;

	result += PrintLine("matrix c_diffuse%dTextureMatrix;", index);

	return result;
}

std::string CDx11EffectGenerator::GenerateDiffuseMapSampling(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source, DIFFUSE_MAP_COMBINE_MODE combineMode)
{
	std::string result;

	switch(source)
	{
	case DIFFUSE_MAP_COORD_UV0:
	case DIFFUSE_MAP_COORD_UV1:
		result += PrintLine("	float4 diffuseColor%d = c_diffuse%dTexture.Sample(c_diffuse%dSampler, input.diffuseCoord%d);", index, index, index, index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_POS:
	case DIFFUSE_MAP_COORD_CUBE_REFLECT:
		result += PrintLine("	float4 diffuseColor%d = texCUBE(c_diffuse%dSampler, input.diffuseCoord%d);", index, index, index);
		break;
	}

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
