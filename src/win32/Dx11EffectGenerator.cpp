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
	const char* shader =
		"cbuffer MatrixBuffer\r\n"
		"{\r\n"
		"	matrix worldMatrix;\r\n"
		"	matrix viewProjMatrix;\r\n"
		"};\r\n"

		"struct VertexInputType\r\n"
		"{\r\n"
		"	float3 position : POSITION;\r\n"
		"};\r\n"

		"struct PixelInputType\r\n"
		"{\r\n"
		"	float4 position : SV_POSITION;\r\n"
		"	float4 color : COLOR;\r\n"
		"};\r\n"

		"PixelInputType VertexProgram(VertexInputType input)\r\n"
		"{\r\n"
		"	PixelInputType output;\r\n"
		"\r\n"
		"	output.position = mul(worldMatrix, float4(input.position, 1));\r\n"
		"	output.position = mul(viewProjMatrix, output.position);\r\n"
		"\r\n"
		"	output.color = float4(1, 1, 1, 1);\r\n"
		"\r\n"
		"	return output;\r\n"
		"}\r\n";

	return shader;
}

std::string CDx11EffectGenerator::GeneratePixelShader(const EFFECTCAPS& caps)
{
	const char* shader = 
		"struct PixelInputType\r\n"
		"{\r\n"
		"	float4 position : SV_POSITION;\r\n"
		"	float4 color : COLOR;\r\n"
		"};\r\n"
		"\r\n"
		"float4 PixelProgram(PixelInputType input) : SV_TARGET\r\n"
		"{\r\n"
		"	return input.color;\r\n"
		"}\r\n";
	return shader;
}

bool CDx11EffectGenerator::HasCoordSrc(const EFFECTCAPS& caps, unsigned int texCoordSource)
{
	return 
		(caps.diffuseMap0CoordSrc == texCoordSource) || 
		(caps.diffuseMap1CoordSrc == texCoordSource) || 
		(caps.diffuseMap2CoordSrc == texCoordSource) || 
		(caps.diffuseMap3CoordSrc == texCoordSource) || 
		(caps.diffuseMap4CoordSrc == texCoordSource);
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
	result += PrintLine("int c_diffuse%dTextureAddressModeU = 0;", index);
	result += PrintLine("int c_diffuse%dTextureAddressModeV = 0;", index);

	result += PrintLine("sampler c_diffuse%dSampler = sampler_state", index);
	result += PrintLine("{");
	result += PrintLine("	Texture = <c_diffuse%dTexture>;", index);
	result += PrintLine("	AddressU = <c_diffuse%dTextureAddressModeU>;", index);
	result += PrintLine("	AddressV = <c_diffuse%dTextureAddressModeV>;", index);
	result += PrintLine("};");

	result += PrintLine("float4x4 c_diffuse%dTextureMatrix;", index);

	return result;
}

std::string CDx11EffectGenerator::GenerateDiffuseMapSampling(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source, DIFFUSE_MAP_COMBINE_MODE combineMode)
{
	std::string result;

	switch(source)
	{
	case DIFFUSE_MAP_COORD_UV0:
	case DIFFUSE_MAP_COORD_UV1:
		result += PrintLine("	float4 diffuseColor%d = tex2D(c_diffuse%dSampler, input.diffuseCoord%d);", index, index, index);
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
