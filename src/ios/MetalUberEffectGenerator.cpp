#include "palleon/ios/MetalUberEffectGenerator.h"

using namespace Palleon;

std::string PrintLine(const char* format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 256, format, args);
	va_end(args);
	return std::string(buffer) + std::string("\n");
}

std::string CMetalUberEffectGenerator::GenerateLibrarySource(const EFFECTCAPS& caps)
{
	std::string result;
	
	result += PrintLine("#include <metal_stdlib>");
	result += PrintLine("#include <simd/simd.h>");
	
	result += PrintLine("using namespace metal;");
	
	result += PrintLine("struct Uniforms");
	result += PrintLine("{");
	result += PrintLine("\tmatrix_float4x4 modelViewProjMatrix;");
	result += PrintLine("\tfloat4 meshColor;");
	result += PrintLine("};");
	
	result += PrintLine("struct VertexIn");
	result += PrintLine("{");
	result += PrintLine("\tpacked_float3 position;");
	if(caps.hasNormal)
	{
		result += PrintLine("\tpacked_float3 normal;");
	}
	if(caps.hasTexCoord0)
	{
		result += PrintLine("\tpacked_float2 texCoord0;");
	}
	result += PrintLine("};");
	
	result += PrintLine("struct VertexOut");
	result += PrintLine("{");
	result += PrintLine("\tfloat4 position [[position]];");
	result += PrintLine("\tfloat2 texCoord0;");
	result += PrintLine("\thalf4 color;");
	result += PrintLine("};");
	
	result += PrintLine("vertex VertexOut VertexShader(");
	result += PrintLine("\tunsigned int vertexId [[vertex_id]],");
	result += PrintLine("\tglobal VertexIn* vertices [[buffer(0)]],");
	result += PrintLine("\tconstant Uniforms& uniforms [[buffer(1)]]");
	result += PrintLine(")");
	result += PrintLine("{");
	result += PrintLine("\tVertexOut out;");
	result += PrintLine("\tfloat4 position = float4(float3(vertices[vertexId].position), 1.0);");
	result += PrintLine("\tout.position = uniforms.modelViewProjMatrix * position;");
	result += PrintLine("\tout.color = half4(uniforms.meshColor);");
	if(caps.hasTexture)
	{
		result += PrintLine("\tout.texCoord0 = vertices[vertexId].texCoord0;");
	}
	result += PrintLine("\treturn out;");
	result += PrintLine("}");
	
	result += PrintLine("fragment half4 FragmentShader(");
	if(caps.hasTexture)
	{
		result += PrintLine("\ttexture2d<half> texture0 [[texture(0)]], ");
		result += PrintLine("\tsampler smp0 [[sampler(0)]],");
	}
	result += PrintLine("\tVertexOut input [[stage_in]])");
	result += PrintLine("{");
	if(caps.hasTexture)
	{
		result += PrintLine("\treturn input.color * texture0.sample(smp0, input.texCoord0);");
	}
	else
	{
		result += PrintLine("\treturn input.color;");
	}
	result += PrintLine("}");
	
	return result;
}
