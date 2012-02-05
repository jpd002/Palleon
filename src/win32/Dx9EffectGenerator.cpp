#include "athena/win32/Dx9EffectGenerator.h"
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

std::string CDx9EffectGenerator::GenerateEffect(const EFFECTCAPS& caps)
{
	std::string result;

	//Uniform definitions
	//-----------------------------
	result += PrintLine("float4x4 c_viewProjMatrix;");
	result += PrintLine("float4x4 c_worldMatrix;");
	result += PrintLine("float4 c_meshColor;");

	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapSampler(0);
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapSampler(1);
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapSampler(2);
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapSampler(3);
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapSampler(4);

	bool needsTexCoord0 = HasCoordSrc(caps, DIFFUSE_MAP_COORD_UV0);
	bool needsTexCoord1 = HasCoordSrc(caps, DIFFUSE_MAP_COORD_UV1);

	//Structure definitions
	//-----------------------------
	result += PrintLine("struct INPUT");
	result += PrintLine("{");
	result += PrintLine("float3		position : POSITION;");
	if(caps.hasVertexColor)
	{
		result += PrintLine("float4		color : COLOR;");
	}
	if(needsTexCoord0)
	{
		result += PrintLine("float2		texCoord0 : TEXCOORD0;");
	}
	if(needsTexCoord1)
	{
		result += PrintLine("float2		texCoord1 : TEXCOORD1;");
	}
	result += PrintLine("};");

	result += PrintLine("struct OUTPUT");
	result += PrintLine("{");
	result += PrintLine("float4		position : POSITION;");
	result += PrintLine("float4		color : COLOR;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordOutput(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapCoordOutput(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapCoordOutput(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapCoordOutput(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapCoordOutput(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));
	result += PrintLine("};");

	//Vertex Shader
	//-----------------------------
	result += PrintLine("OUTPUT VShader(INPUT input)");
	result += PrintLine("{");
	result += PrintLine("	float4 worldPos = mul(float4(input.position, 1), c_worldMatrix);");
	result += PrintLine("	OUTPUT output;");
	result += PrintLine("	output.position = mul(worldPos, c_viewProjMatrix);");
	if(caps.hasVertexColor)
	{
		result += PrintLine("	output.color = input.color * c_meshColor;");
	}
	else
	{
		result += PrintLine("	output.color = c_meshColor;");
	}
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordComputation(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapCoordComputation(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapCoordComputation(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapCoordComputation(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapCoordComputation(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));
	result += PrintLine("	return output;");
	result += PrintLine("}");

	//Pixel Shader
	//-----------------------------
	result += PrintLine("float4 PShader(OUTPUT input) : COLOR");
	result += PrintLine("{");
	result += PrintLine("	float4 diffuseColor = input.color;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapSampling(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap0CombineMode));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapSampling(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap1CombineMode));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapSampling(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap2CombineMode));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapSampling(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap3CombineMode));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapSampling(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap4CombineMode));
	result += PrintLine("	return diffuseColor;");
	result += PrintLine("}");

	//Technique definition
	//-----------------------------
	result += PrintLine("technique DrawMesh");
	result += PrintLine("{");
	result += PrintLine("	pass P0");
	result += PrintLine("	{");
	result += PrintLine("		VertexShader = compile vs_3_0 VShader();");
	result += PrintLine("		PixelShader = compile ps_3_0 PShader();");
	result += PrintLine("	}");
	result += PrintLine("}");

	return result;
}

bool CDx9EffectGenerator::HasCoordSrc(const EFFECTCAPS& caps, unsigned int texCoordSource)
{
	return 
		(caps.diffuseMap0CoordSrc == texCoordSource) || 
		(caps.diffuseMap1CoordSrc == texCoordSource) || 
		(caps.diffuseMap2CoordSrc == texCoordSource) || 
		(caps.diffuseMap3CoordSrc == texCoordSource) || 
		(caps.diffuseMap4CoordSrc == texCoordSource);
}

std::string CDx9EffectGenerator::GenerateDiffuseMapCoordOutput(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;
	switch(source)
	{
	case DIFFUSE_MAP_COORD_UV0:
	case DIFFUSE_MAP_COORD_UV1:
		result += PrintLine("float2		diffuseCoord%d : TEXCOORD%d;", index, index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_POS:
		result += PrintLine("float3		diffuseCoord%d : TEXCOORD%d;", index, index);
		break;
	}
	return result;
}

std::string CDx9EffectGenerator::GenerateDiffuseMapCoordComputation(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
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
		result += PrintLine("	output.diffuseCoord%d = normalize(worldPos).xyz;", index, index);
		break;
	}

	return result;
}

std::string CDx9EffectGenerator::GenerateDiffuseMapSampler(unsigned int index)
{
	std::string result;

	result += PrintLine("texture2D c_diffuse%dTexture;", index);

	result += PrintLine("sampler c_diffuse%dSampler = sampler_state", index);
	result += PrintLine("{");
	result += PrintLine("	texture = <c_diffuse%dTexture>;", index);
	result += PrintLine("};");

	result += PrintLine("float4x4 c_diffuse%dTextureMatrix;", index);

	return result;
}

std::string CDx9EffectGenerator::GenerateDiffuseMapSampling(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source, DIFFUSE_MAP_COMBINE_MODE combineMode)
{
	std::string result;

	switch(source)
	{
	case DIFFUSE_MAP_COORD_UV0:
	case DIFFUSE_MAP_COORD_UV1:
		result += PrintLine("	float4 diffuseColor%d = tex2D(c_diffuse%dSampler, input.diffuseCoord%d);", index, index, index);
		break;
	case DIFFUSE_MAP_COORD_CUBE_POS:
		result += PrintLine("	float4 diffuseColor%d = texCUBE(c_diffuse%dSampler, input.diffuseCoord%d);", index, index, index);
		break;
	}

	switch(combineMode)
	{
	case DIFFUSE_MAP_COMBINE_MODULATE:
		result += PrintLine("	diffuseColor *= diffuseColor%d;", index);
		break;
	case DIFFUSE_MAP_COMBINE_LERP:
		result += PrintLine("	diffuseColor = lerp(diffuseColor, diffuseColor%d, diffuseColor%d.a);", index, index);
		break;
	}

	return result;
}
