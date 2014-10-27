#include "WaterEffect.h"

static const char* g_cameraPositionUniformName = "v_cameraPosition";
static const char* g_reflectViewProjMatrix = "v_reflectViewProjMatrix";

CWaterEffect::CWaterEffect()
: Palleon::CPlatformGenericEffect(CreateInputBindings(), CreateVertexShader(), CreatePixelShader())
{

}

Palleon::EffectInputBindingArray CWaterEffect::CreateInputBindings()
{
	Palleon::EffectInputBindingArray inputBindings;
	inputBindings.push_back(Palleon::EFFECTINPUTBINDING(Palleon::VERTEX_ITEM_ID_POSITION,	Palleon::SEMANTIC_POSITION, 0));
	inputBindings.push_back(Palleon::EFFECTINPUTBINDING(Palleon::VERTEX_ITEM_ID_UV0,		Palleon::SEMANTIC_TEXCOORD, 0));
	return inputBindings;
}

void CWaterEffect::UpdateSpecificConstants(const Palleon::VIEWPORT_PARAMS& viewportParams, Palleon::CMaterial* material, const CMatrix4& worldMatrix)
{
	auto clipPlane = CVector4(0, 1, 0, 0);

	auto reflectMatrix = CMatrix4::MakeReflect(clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);

	auto reflectViewMatrix = reflectMatrix * viewportParams.viewMatrix;

	auto inverseView = viewportParams.viewMatrix.Inverse();
	auto cameraPosition = CVector3(inverseView(3, 0), inverseView(3, 1), inverseView(3, 2));

	SetConstant(g_cameraPositionUniformName, Palleon::CEffectParameter(cameraPosition));
	SetConstant(g_reflectViewProjMatrix, reflectViewMatrix * viewportParams.projMatrix);
}

Palleon::CShaderBuilder CWaterEffect::CreateVertexShader()
{
	Palleon::CShaderBuilder b;

	//Inputs
	const auto inputPosition = b.CreateInputFloat3(Palleon::SEMANTIC_POSITION);
	const auto inputTexCoord = b.CreateInputFloat2(Palleon::SEMANTIC_TEXCOORD);

	//Outputs
	const auto outputPosition = b.CreateOutputFloat4(Palleon::SEMANTIC_SYSTEM_POSITION);
	const auto outputReflectPosition = b.CreateOutputFloat4(Palleon::SEMANTIC_TEXCOORD, 0);
	const auto outputRefractPosition = b.CreateOutputFloat4(Palleon::SEMANTIC_TEXCOORD, 1);
	const auto outputEyeVector = b.CreateOutputFloat3(Palleon::SEMANTIC_TEXCOORD, 2);
	const auto outputBumpTexCoord0 = b.CreateOutputFloat2(Palleon::SEMANTIC_TEXCOORD, 3);
	const auto outputBumpTexCoord1 = b.CreateOutputFloat2(Palleon::SEMANTIC_TEXCOORD, 4);
		
	//Uniforms
	const auto worldMatrix = b.CreateUniformMatrix(g_worldMatrixName);
	const auto viewProjMatrix = b.CreateUniformMatrix(g_viewProjMatrixName);
	const auto reflectViewProjMatrix = b.CreateUniformMatrix(g_reflectViewProjMatrix);
	const auto texture2Matrix = b.CreateUniformMatrix(g_texture2MatrixName);
	const auto texture3Matrix = b.CreateUniformMatrix(g_texture3MatrixName);
	const auto cameraPosition = b.CreateUniformFloat3(g_cameraPositionUniformName);

	auto worldPos = b.Multiply(worldMatrix, b.NewFloat4(inputPosition, b.CreateConstant(1.f)));
	auto screenPos = b.Multiply(viewProjMatrix, worldPos);
	auto bumpTexCoord0 = b.Multiply(texture2Matrix, b.NewFloat4(inputTexCoord, b.CreateConstant(0), b.CreateConstant(1)));
	auto bumpTexCoord1 = b.Multiply(texture3Matrix, b.NewFloat4(inputTexCoord, b.CreateConstant(0), b.CreateConstant(1)));
	
	auto eyeVector = 
		b.Normalize(
			b.Substract(
				cameraPosition, 
				b.SwizzleFloat3(worldPos, Palleon::SWIZZLE_XYZ)
			)
		);

	b.Assign(outputPosition, screenPos);
	b.Assign(outputEyeVector, eyeVector);
	b.Assign(outputBumpTexCoord0, b.SwizzleFloat2(bumpTexCoord0, Palleon::SWIZZLE_XY));
	b.Assign(outputBumpTexCoord1, b.SwizzleFloat2(bumpTexCoord1, Palleon::SWIZZLE_XY));
	b.Assign(outputReflectPosition, b.Multiply(reflectViewProjMatrix, worldPos));
	b.Assign(outputRefractPosition, screenPos);
	
	return b;
}

Palleon::CShaderBuilder CWaterEffect::CreatePixelShader()
{
	Palleon::CShaderBuilder b;

	//Inputs
	const auto inputPosition = b.CreateInputFloat4(Palleon::SEMANTIC_SYSTEM_POSITION);
	const auto inputReflectPosition = b.CreateInputFloat4(Palleon::SEMANTIC_TEXCOORD, 0);
	const auto inputRefractPosition = b.CreateInputFloat4(Palleon::SEMANTIC_TEXCOORD, 1);
	const auto inputEyeVector = b.CreateInputFloat3(Palleon::SEMANTIC_TEXCOORD, 2);
	const auto inputBumpTexCoord0 = b.CreateInputFloat2(Palleon::SEMANTIC_TEXCOORD, 3);
	const auto inputBumpTexCoord1 = b.CreateInputFloat2(Palleon::SEMANTIC_TEXCOORD, 4);

	//Outputs
	const auto outputColor = b.CreateOutputFloat4(Palleon::SEMANTIC_SYSTEM_COLOR);

	//Uniforms
	const auto reflectTexture = b.CreateTexture2D(0);
	const auto refractTexture = b.CreateTexture2D(1);
	const auto bumpTexture0 = b.CreateTexture2D(2);
	const auto bumpTexture1 = b.CreateTexture2D(3);

	//Code
	float waveLength = 0.03f;

	auto lightDir = b.Normalize(b.CreateConstant(-1.0f, 1.0f, 1.0f));
	auto dullColor = b.CreateConstant(0.2f, 0.1f, 0.0f, 1.0f);

	auto bump0 = b.Sample(bumpTexture0, inputBumpTexCoord0);
	auto bump1 = b.Sample(bumpTexture1, inputBumpTexCoord1);

	auto normal = 
		b.Multiply(
			b.Substract(
				b.Multiply(
					b.Add(
						b.SwizzleFloat3(bump0, Palleon::SWIZZLE_XYZ),
						b.SwizzleFloat3(bump1, Palleon::SWIZZLE_XYZ)
					),
					b.CreateConstant(0.5f, 0.5f, 0.5f)
				),
				b.CreateConstant(0.5f, 0.5f, 0.5f)
			),
			b.CreateConstant(2.0f, 2.0f, 2.0f)
		);

	normal = b.Normalize(normal);

	auto perturbation = 
		b.Multiply(
			b.SwizzleFloat2(normal, Palleon::SWIZZLE_XZ),
			b.CreateConstant(waveLength, waveLength)
		);

	auto reflectTexCoordX = 
		b.Add(
			b.Divide(
				b.Divide(
					b.SwizzleFloat(inputReflectPosition, Palleon::SWIZZLE_X),
					b.SwizzleFloat(inputReflectPosition, Palleon::SWIZZLE_W)
				),
				b.CreateConstant(2.0f)
			),
			b.CreateConstant(0.5f)
		);
	auto refractTexCoordX = 
		b.Add(
			b.Divide(
				b.Divide(
					b.SwizzleFloat(inputRefractPosition, Palleon::SWIZZLE_X),
					b.SwizzleFloat(inputRefractPosition, Palleon::SWIZZLE_W)
				),
				b.CreateConstant(2.0f)
			),
			b.CreateConstant(0.5f)
		);

	auto reflectTexCoordY = 
		b.Add(
			b.Divide(
				b.Divide(
#if defined(PALLEON_WIN32)
					b.Negate(b.SwizzleFloat(inputReflectPosition, Palleon::SWIZZLE_Y)),
#elif defined(PALLEON_IOS) || defined(PALLEON_ANDROID)
					b.SwizzleFloat(inputReflectPosition, Palleon::SWIZZLE_Y),
#endif
					b.SwizzleFloat(inputReflectPosition, Palleon::SWIZZLE_W)
				),
				b.CreateConstant(2.0f)
			),
			b.CreateConstant(0.5f)
		);
	auto refractTexCoordY = 
		b.Add(
			b.Divide(
				b.Divide(
#if defined(PALLEON_WIN32)
					b.Negate(b.SwizzleFloat(inputRefractPosition, Palleon::SWIZZLE_Y)),
#elif defined(PALLEON_IOS) || defined(PALLEON_ANDROID)
					b.SwizzleFloat(inputRefractPosition, Palleon::SWIZZLE_Y),
#endif
					b.SwizzleFloat(inputRefractPosition, Palleon::SWIZZLE_W)
				),
				b.CreateConstant(2.0f)
			),
			b.CreateConstant(0.5f)
		);

	auto reflectTexCoord = b.NewFloat2(reflectTexCoordX, reflectTexCoordY);
	auto refractTexCoord = b.NewFloat2(refractTexCoordX, refractTexCoordY);

	reflectTexCoord = b.Add(reflectTexCoord, perturbation);
	refractTexCoord = b.Add(refractTexCoord, perturbation);

	auto reflectColor = b.Sample(reflectTexture, reflectTexCoord);
	auto refractColor = b.Sample(refractTexture, refractTexCoord);

	auto fresnelTerm = b.Saturate(b.Multiply(b.Dot(inputEyeVector, normal), b.CreateConstant(1.3f)));
	auto halfVector = b.Normalize(b.Add(inputEyeVector, lightDir));
	auto specularFactor = b.Max(b.Dot(normal, halfVector), b.CreateConstant(0));
	auto specular = b.Pow(specularFactor, b.CreateConstant(15.f));

	auto diffuse = b.Max(b.Dot(normal, lightDir), b.CreateConstant(0));

	auto combinedColor = b.Mix(reflectColor, refractColor, fresnelTerm);
	combinedColor = b.Mix(combinedColor, dullColor, b.CreateConstant(0.2f));

	auto specularColor = b.NewFloat4(specular, specular, specular, b.CreateConstant(1));
	auto diffuseColor = 
		b.Multiply(
			b.NewFloat4(diffuse, diffuse, diffuse, b.CreateConstant(1)),
			b.CreateConstant(0.1f, 0.1f, 0.1f, 1.0f)
		);
	auto lightColor = b.Add(diffuseColor, specularColor);

	b.Assign(outputColor, b.Add(combinedColor, lightColor));

	return b;
}
