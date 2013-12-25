#include "athena/ios/IosUberEffectGenerator.h"

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

std::string CIosUberEffectGenerator::GenerateVertexShader(const EFFECTCAPS& caps)
{
	bool needsTexCoord0 = HasCoordSrc(caps, DIFFUSE_MAP_COORD_UV0);
	bool needsTexCoord1 = HasCoordSrc(caps, DIFFUSE_MAP_COORD_UV1);
	
	std::string result;
	
	//Attributes
	result += PrintLine("attribute vec3 a_position;");
	if(caps.hasVertexColor)	result += PrintLine("attribute vec4 a_color;");
	if(needsTexCoord0)		result += PrintLine("attribute vec2 a_texCoord0;");
	if(needsTexCoord1)		result += PrintLine("attribute vec2 a_texCoord1;");
	
	//Varyings
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordOutput(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapCoordOutput(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapCoordOutput(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapCoordOutput(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapCoordOutput(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));
	result += PrintLine("varying lowp vec4 v_color;");
	
	//Uniforms
	result += PrintLine("uniform mat4 c_viewProjMatrix;");
	result += PrintLine("uniform mat4 c_worldMatrix;");
	
	result += PrintLine("uniform vec4 c_meshColor;");
	
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapMatrixUniform(0);
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapMatrixUniform(1);
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapMatrixUniform(2);
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapMatrixUniform(3);
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapMatrixUniform(4);
	
	result += PrintLine("void main()");
	result += PrintLine("{");
	result += PrintLine("	vec4 worldPos = c_worldMatrix * vec4(a_position, 1);");
	result += PrintLine("	gl_Position = c_viewProjMatrix * worldPos;");
	if(caps.hasVertexColor)
	{
		result += PrintLine("v_color = a_color * c_meshColor;");
	}
	else
	{
		result += PrintLine("v_color = c_meshColor;");
	}
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordComputation(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapCoordComputation(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapCoordComputation(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapCoordComputation(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapCoordComputation(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc));
	result += PrintLine("}");
	
	return result;
}

std::string CIosUberEffectGenerator::GeneratePixelShader(const EFFECTCAPS& caps)
{
	std::string result;

	result += PrintLine("varying lowp vec4 v_color;");
	if(caps.hasDiffuseMap0) { result += GenerateDiffuseMapSampler(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc)); result += GenerateDiffuseMapCoordOutput(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc)); }
	if(caps.hasDiffuseMap1) { result += GenerateDiffuseMapSampler(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc)); result += GenerateDiffuseMapCoordOutput(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc)); }
	if(caps.hasDiffuseMap2) { result += GenerateDiffuseMapSampler(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc)); result += GenerateDiffuseMapCoordOutput(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc)); }
	if(caps.hasDiffuseMap3) { result += GenerateDiffuseMapSampler(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc)); result += GenerateDiffuseMapCoordOutput(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc)); }
	if(caps.hasDiffuseMap4) { result += GenerateDiffuseMapSampler(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc)); result += GenerateDiffuseMapCoordOutput(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc)); }
	
	result += PrintLine("void main()");
	result += PrintLine("{");
	result += PrintLine("	lowp vec4 diffuseColor = v_color;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapSampling(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap0CombineMode));
	if(caps.hasDiffuseMap1) result += GenerateDiffuseMapSampling(1, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap1CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap1CombineMode));
	if(caps.hasDiffuseMap2) result += GenerateDiffuseMapSampling(2, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap2CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap2CombineMode));
	if(caps.hasDiffuseMap3) result += GenerateDiffuseMapSampling(3, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap3CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap3CombineMode));
	if(caps.hasDiffuseMap4) result += GenerateDiffuseMapSampling(4, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap4CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap4CombineMode));	
	result += PrintLine("	gl_FragColor = diffuseColor;");
	result += PrintLine("}");
	
	return result;
}

bool CIosUberEffectGenerator::HasCoordSrc(const EFFECTCAPS& caps, unsigned int texCoordSource)
{
	return
		(caps.diffuseMap0CoordSrc == texCoordSource) ||
		(caps.diffuseMap1CoordSrc == texCoordSource) ||
		(caps.diffuseMap2CoordSrc == texCoordSource) ||
		(caps.diffuseMap3CoordSrc == texCoordSource) ||
		(caps.diffuseMap4CoordSrc == texCoordSource);
}

std::string CIosUberEffectGenerator::GenerateDiffuseMapCoordOutput(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;
	switch(source)
	{
		case DIFFUSE_MAP_COORD_UV0:
		case DIFFUSE_MAP_COORD_UV1:
			result += PrintLine("varying mediump vec2 v_diffuseCoord%d;", index);
			break;
		case DIFFUSE_MAP_COORD_CUBE_POS:
		case DIFFUSE_MAP_COORD_CUBE_REFLECT:
			result += PrintLine("varying mediump vec3 v_diffuseCoord%d;", index);
			break;
	}
	return result;
}

std::string CIosUberEffectGenerator::GenerateDiffuseMapCoordComputation(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;
	switch(source)
	{
		case DIFFUSE_MAP_COORD_UV0:
			result += PrintLine("v_diffuseCoord%d = (c_diffuseTextureMatrix%d * vec4(a_texCoord0, 0, 1)).xy;", index, index);
			break;
		case DIFFUSE_MAP_COORD_UV1:
			result += PrintLine("v_diffuseCoord%d = (c_diffuseTextureMatrix%d * vec4(a_texCoord1, 0, 1)).xy;", index, index);
			break;
		case DIFFUSE_MAP_COORD_CUBE_POS:
			result += PrintLine("v_diffuseCoord%d = normalize(worldPos.xyz);", index);
			break;
	}
	return result;
}

std::string CIosUberEffectGenerator::GenerateDiffuseMapSampler(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;
	switch(source)
	{
		case DIFFUSE_MAP_COORD_UV0:
		case DIFFUSE_MAP_COORD_UV1:
			result += PrintLine("uniform sampler2D c_diffuseTexture%d;", index);
			break;
		case DIFFUSE_MAP_COORD_CUBE_POS:
		case DIFFUSE_MAP_COORD_CUBE_REFLECT:
			result += PrintLine("uniform samplerCube c_diffuseTexture%d;", index);			
			break;
	}
	return result;
}

std::string CIosUberEffectGenerator::GenerateDiffuseMapMatrixUniform(unsigned int index)
{
	std::string result;
	result += PrintLine("uniform mat4 c_diffuseTextureMatrix%d;", index);
	return result;
}

std::string CIosUberEffectGenerator::GenerateDiffuseMapSampling(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source, DIFFUSE_MAP_COMBINE_MODE combineMode)
{
	std::string result;
	
	switch(source)
	{
		case DIFFUSE_MAP_COORD_UV0:
		case DIFFUSE_MAP_COORD_UV1:
			result += PrintLine("lowp vec4 diffuseColor%d = texture2D(c_diffuseTexture%d, v_diffuseCoord%d);", index, index, index);
			break;
			
		case DIFFUSE_MAP_COORD_CUBE_POS:
		case DIFFUSE_MAP_COORD_CUBE_REFLECT:
			result += PrintLine("lowp vec4 diffuseColor%d = textureCube(c_diffuseTexture%d, v_diffuseCoord%d);", index, index, index);
			break;
	}
	
	switch(combineMode)
	{
		case DIFFUSE_MAP_COMBINE_MODULATE:
			result += PrintLine("diffuseColor *= diffuseColor%d;", index);
			break;
		case DIFFUSE_MAP_COMBINE_ADD:
			result += PrintLine("diffuseColor += diffuseColor%d;", index);
			break;
		case DIFFUSE_MAP_COMBINE_LERP:
			result += PrintLine("diffuseColor = mix(diffuseColor, diffuseColor%d, diffuseColor%d.a);", index, index);
			break;
	}
	
	return result;
}