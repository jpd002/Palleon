#include "athena/iphone/IphoneEffectGenerator.h"

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

std::string CIphoneEffectGenerator::GenerateVertexShader(const EFFECTCAPS& caps)
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
	result += PrintLine("varying lowp vec4 v_color;");
	
	//Uniforms
	result += PrintLine("uniform mat4 c_viewProjMatrix;");
	result += PrintLine("uniform mat4 c_worldMatrix;");
	
	result += PrintLine("uniform vec4 c_meshColor;");
	
	result += PrintLine("void main()");
	result += PrintLine("{");
	result += PrintLine("	gl_Position = c_viewProjMatrix * c_worldMatrix * vec4(a_position, 1);");
	if(caps.hasVertexColor)
	{
		result += PrintLine("v_color = a_color * c_meshColor;");
	}
	else
	{
		result += PrintLine("v_color = c_meshColor;");
	}
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapCoordComputation(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc));
	result += PrintLine("}");
	
	return result;
}

std::string CIphoneEffectGenerator::GeneratePixelShader(const EFFECTCAPS& caps)
{
	std::string result;

	result += PrintLine("varying lowp vec4 v_color;");
	if(caps.hasDiffuseMap0) { result += GenerateDiffuseMapSampler(0); result += GenerateDiffuseMapCoordOutput(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc)); }
	
	result += PrintLine("void main()");
	result += PrintLine("{");
	result += PrintLine("	lowp vec4 diffuseColor = v_color;");
	if(caps.hasDiffuseMap0) result += GenerateDiffuseMapSampling(0, static_cast<DIFFUSE_MAP_COORD_SOURCE>(caps.diffuseMap0CoordSrc), static_cast<DIFFUSE_MAP_COMBINE_MODE>(caps.diffuseMap0CombineMode));
	result += PrintLine("	gl_FragColor = diffuseColor;");
	result += PrintLine("}");
	
	return result;
}

bool CIphoneEffectGenerator::HasCoordSrc(const EFFECTCAPS& caps, unsigned int texCoordSource)
{
	return
		(caps.diffuseMap0CoordSrc == texCoordSource) ||
		(caps.diffuseMap1CoordSrc == texCoordSource) ||
		(caps.diffuseMap2CoordSrc == texCoordSource) ||
		(caps.diffuseMap3CoordSrc == texCoordSource) ||
		(caps.diffuseMap4CoordSrc == texCoordSource);
}

std::string CIphoneEffectGenerator::GenerateDiffuseMapCoordOutput(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;
	switch(source)
	{
		case DIFFUSE_MAP_COORD_UV0:
		case DIFFUSE_MAP_COORD_UV1:
			result += PrintLine("varying mediump vec2 v_diffuseCoord%d;", index);
			break;
	}
	return result;
}

std::string CIphoneEffectGenerator::GenerateDiffuseMapCoordComputation(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source)
{
	std::string result;
	switch(source)
	{
		case DIFFUSE_MAP_COORD_UV0:
			result += PrintLine("v_diffuseCoord%d = a_texCoord0;", index);
			break;
	}
	return result;
}

std::string CIphoneEffectGenerator::GenerateDiffuseMapSampler(unsigned int index)
{
	std::string result;
	result += PrintLine("uniform sampler2D c_diffuseTexture%d;", index);
	return result;
}

std::string CIphoneEffectGenerator::GenerateDiffuseMapSampling(unsigned int index, DIFFUSE_MAP_COORD_SOURCE source, DIFFUSE_MAP_COMBINE_MODE combineMode)
{
	std::string result;
	
	switch(source)
	{
		case DIFFUSE_MAP_COORD_UV0:
			result += PrintLine("lowp vec4 diffuseColor%d = texture2D(c_diffuseTexture%d, v_diffuseCoord%d);", index, index, index);
			break;
	}
	
	switch(combineMode)
	{
		case DIFFUSE_MAP_COMBINE_MODULATE:
			result += PrintLine("diffuseColor *= diffuseColor%d;", index);
			break;
	}
	
	return result;
}