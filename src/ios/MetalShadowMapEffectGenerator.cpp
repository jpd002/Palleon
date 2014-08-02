#include "TargetConditionals.h"
#if !TARGET_IPHONE_SIMULATOR

#include "palleon/ios/MetalShadowMapEffectGenerator.h"

using namespace Palleon;

std::string CMetalShadowMapEffectGenerator::GenerateLibrarySource(const CMetalShadowMapEffectGenerator::EFFECTCAPS& caps)
{
	std::string result;
	
	result += "#include <metal_stdlib>\n";
	result += "#include <simd/simd.h>\n";
	
	result += "using namespace metal;\n";
	
	result += "struct Uniforms\n";
	result += "{\n";
	result += "matrix_float4x4 worldViewProjMatrix;\n";
	result += "};\n";
	
	result += "struct VertexIn\n";
	result += "{\n";
	result += "packed_float3 position;\n";
	if(caps.hasNormal)
	{
		result += "packed_float3 normal;\n";
	}
	if(caps.hasTexCoord0)
	{
		result += "packed_float2 texCoord0;\n";
	}
	result += "};\n";

	result += "struct VertexOut\n";
	result += "{\n";
	result += "float4 position [[position]];\n";
	result += "};";
	
	result += "vertex VertexOut VertexShader(";
	result += "unsigned int vertexId [[vertex_id]],\n";
	result += "device VertexIn* vertices [[buffer(0)]],\n";
	result += "constant Uniforms& uniforms [[buffer(1)]]\n";
	result += ")\n";
	result += "{\n";
	result += "VertexOut out;\n";
	result += "float4 position = float4(float3(vertices[vertexId].position), 1.0);\n";
	result += "out.position = uniforms.worldViewProjMatrix * position;\n";
	result += "return out;\n";
	result += "}\n";
	
	result += "fragment half4 FragmentShader(\n";
	result += "VertexOut input [[stage_in]])\n";
	result += "{\n";
	result += "\treturn half4(0, 0, 0, 0);\n";
	result += "}\n";
	
	return result;
}

#endif

