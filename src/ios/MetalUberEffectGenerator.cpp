#include "palleon/ios/MetalUberEffectGenerator.h"

using namespace Palleon;

std::string CMetalUberEffectGenerator::GenerateLibrarySource(const EFFECTCAPS& caps)
{
	std::string result;
	
	result += "#include <metal_stdlib>\n";
	result += "#include <simd/simd.h>\n";
	
	result += "using namespace metal;\n";
	
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
	result += "};";
	
	result += "struct VertexOut";
	result += "{";
	result += "float4 position [[position]];\n";
	result += "float2 texCoord0;\n";
	result += "half4 color;\n";
	if(caps.hasShadowMap)
	{
		result += "float4 shadowPosition;\n";
	}
	result += "};\n";
	
	////////////////////////////////////////////////////
	//Vertex Shader
	////////////////////////////////////////////////////
	
	result += "struct VertexUniforms\n";
	result += "{\n";
	result += "matrix_float4x4 worldMatrix;\n";
	result += "matrix_float4x4 viewProjMatrix;\n";
	if(caps.hasShadowMap)
	{
		result += "matrix_float4x4 shadowViewProjMatrix;\n";
	}
	result += "float4 meshColor;\n";
	result += "};\n";
	
	result += "vertex VertexOut VertexShader(\n";
	result += "unsigned int vertexId [[vertex_id]],\n";
	result += "global VertexIn* vertices [[buffer(0)]],\n";
	result += "constant VertexUniforms& uniforms [[buffer(1)]]\n";
	result += ")\n";
	result += "{\n";
	result += "VertexOut out;\n";
	result += "float4 worldPos = uniforms.worldMatrix * float4(float3(vertices[vertexId].position), 1.0);\n";
	result += "out.position = uniforms.viewProjMatrix * worldPos;\n";
	if(caps.hasShadowMap)
	{
		result += "out.shadowPosition = uniforms.shadowViewProjMatrix * worldPos;\n";
	}
	result += "out.color = half4(uniforms.meshColor);\n";
	if(caps.hasTexture)
	{
		result += "out.texCoord0 = vertices[vertexId].texCoord0;\n";
	}
	result += "return out;\n";
	result += "}\n";
	
	////////////////////////////////////////////////////
	//Fragment Shader
	////////////////////////////////////////////////////
	
	if(caps.hasShadowMap)
	{
		result += "constexpr sampler g_shadowSampler(compare_func::less, min_filter::linear, mag_filter::linear);\n";
	}
	
	result += "fragment half4 FragmentShader(\n";
	if(caps.hasTexture)
	{
		result += "texture2d<half> texture0 [[texture(0)]],\n";
		result += "sampler smp0 [[sampler(0)]],\n";
	}
	if(caps.hasShadowMap)
	{
		result += "depth2d<float> shadowTexture [[texture(1)]]\n,";
	}
	result += "VertexOut input [[stage_in]]\n";
	result += ")\n";
	result += "{\n";
	result += "half4 diffuseColor = input.color;\n";
	if(caps.hasShadowMap)
	{
		result += "float2 shadowMapCoord = 0.5 * (input.shadowPosition.xy / input.shadowPosition.w) + float2(0.5, 0.5);\n";
		result += "float pixelZ = input.shadowPosition.z / input.shadowPosition.w;\n";
		result += "diffuseColor *= shadowTexture.sample_compare(g_shadowSampler, shadowMapCoord, pixelZ);\n";
	}
	if(caps.hasTexture)
	{
		result += "diffuseColor *= texture0.sample(smp0, input.texCoord0);\n";
	}
	result += "return diffuseColor;\n";
	result += "}\n";
	
	return result;
}
