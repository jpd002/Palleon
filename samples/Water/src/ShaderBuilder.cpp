#include "ShaderBuilder.h"

CShaderBuilder::CShaderBuilder()
{

}

CShaderBuilder::~CShaderBuilder()
{

}

const CShaderBuilder::SymbolArray& CShaderBuilder::GetSymbols() const
{
	return m_symbols;
}

CShaderBuilder::SEMANTIC_INFO CShaderBuilder::GetInputSemantic(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_INPUT);
	return m_inputSemantics.find(sym.index)->second;
}

CShaderBuilder::SEMANTIC_INFO CShaderBuilder::GetOutputSemantic(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_OUTPUT);
	return m_outputSemantics.find(sym.index)->second;
}

std::string CShaderBuilder::GetUniformName(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_UNIFORM);
	return m_uniformNames.find(sym.index)->second;
}

CVector4 CShaderBuilder::GetTemporaryValue(const SYMBOL& sym) const
{
	CVector4 result(0, 0, 0, 0);
	assert(sym.location == SYMBOL_LOCATION_TEMPORARY);
	auto temporaryValueIterator = m_temporaryValues.find(sym.index);
	if(temporaryValueIterator != std::end(m_temporaryValues))
	{
		result = temporaryValueIterator->second;
	}
	return result;
}

const CShaderBuilder::StatementList& CShaderBuilder::GetStatements() const
{
	return m_statements;
}

CShaderBuilder::SYMBOLREF_FLOAT CShaderBuilder::CreateTempFloat()
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT;
	sym.location	= SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	SYMBOLREF_FLOAT ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_X;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT2 CShaderBuilder::CreateTempFloat2()
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT2;
	sym.location	= SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	SYMBOLREF_FLOAT2 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XY;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT3 CShaderBuilder::CreateTempFloat3()
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT3;
	sym.location	= SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	SYMBOLREF_FLOAT3 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZ;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::CreateTempFloat4()
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT4;
	sym.location	= SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	SYMBOLREF_FLOAT4 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZW;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT2 CShaderBuilder::CreateInputFloat2(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.index		= m_currentInputIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT2;
	sym.location	= SYMBOL_LOCATION_INPUT;
	m_symbols.push_back(sym);

	m_inputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	SYMBOLREF_FLOAT2 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XY;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT3 CShaderBuilder::CreateInputFloat3(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.index		= m_currentInputIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT3;
	sym.location	= SYMBOL_LOCATION_INPUT;
	m_symbols.push_back(sym);

	m_inputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	SYMBOLREF_FLOAT3 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZ;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::CreateInputFloat4(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.index		= m_currentInputIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT4;
	sym.location	= SYMBOL_LOCATION_INPUT;
	m_symbols.push_back(sym);

	m_inputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	SYMBOLREF_FLOAT4 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZW;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT2 CShaderBuilder::CreateOutputFloat2(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.index		= m_currentOutputIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT2;
	sym.location	= SYMBOL_LOCATION_OUTPUT;
	m_symbols.push_back(sym);

	m_outputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	SYMBOLREF_FLOAT2 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XY;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT3 CShaderBuilder::CreateOutputFloat3(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.index		= m_currentOutputIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT3;
	sym.location	= SYMBOL_LOCATION_OUTPUT;
	m_symbols.push_back(sym);

	m_outputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	SYMBOLREF_FLOAT3 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZ;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::CreateOutputFloat4(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.index		= m_currentOutputIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT4;
	sym.location	= SYMBOL_LOCATION_OUTPUT;
	m_symbols.push_back(sym);

	m_outputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	SYMBOLREF_FLOAT4 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZW;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT CShaderBuilder::CreateConstant(float value)
{
	int tempIndex = 0;
	bool found = false;
	for(const auto& tempValuePair : m_temporaryValues)
	{
		const auto& tempValue = tempValuePair.second;
		if(tempValue.x == value)
		{
			found = true;
			tempIndex = tempValuePair.first;
			break;
		}
	}

	if(!found)
	{
		tempIndex = m_currentTempIndex++;
	}

	SYMBOL sym;
	sym.index		= tempIndex;
	sym.type		= SYMBOL_TYPE_FLOAT;
	sym.location	= SYMBOL_LOCATION_TEMPORARY;
	if(!found)
	{
		m_symbols.push_back(sym);
	}

	auto tempValue = CVector4(value, 0, 0, 0);
	m_temporaryValues.insert(std::make_pair(sym.index, tempValue));

	SYMBOLREF_FLOAT ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_X;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT2 CShaderBuilder::CreateConstant(float v1, float v2)
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT2;
	sym.location	= SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CVector4(v1, v2, 0, 0);
	m_temporaryValues.insert(std::make_pair(sym.index, tempValue));

	SYMBOLREF_FLOAT2 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XY;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT3 CShaderBuilder::CreateConstant(float v1, float v2, float v3)
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT3;
	sym.location	= SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CVector4(v1, v2, v3, 0);
	m_temporaryValues.insert(std::make_pair(sym.index, tempValue));

	SYMBOLREF_FLOAT3 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZ;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::CreateConstant(float v1, float v2, float v3, float v4)
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT;
	sym.location	= SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CVector4(v1, v2, v3, v4);
	m_temporaryValues.insert(std::make_pair(sym.index, tempValue));

	SYMBOLREF_FLOAT4 ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZW;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT3 CShaderBuilder::CreateUniformFloat3(const std::string& name)
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_FLOAT3;
	sym.location	= SYMBOL_LOCATION_UNIFORM;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return SYMBOLREF_FLOAT3(sym, SWIZZLE_XYZ);
}

CShaderBuilder::SYMBOLREF_MATRIX CShaderBuilder::CreateUniformMatrix(const std::string& name)
{
	SYMBOL sym;
	sym.index		= m_currentTempIndex++;
	sym.type		= SYMBOL_TYPE_MATRIX;
	sym.location	= SYMBOL_LOCATION_UNIFORM;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	SYMBOLREF_MATRIX ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZW;
	return ref;
}

CShaderBuilder::SYMBOLREF CShaderBuilder::CreateTexture2D(unsigned int unit)
{
	SYMBOL sym;
	sym.index		= unit;
	sym.type		= SYMBOL_TYPE_TEXTURE2D;
	sym.location	= SYMBOL_LOCATION_TEXTURE;
	m_symbols.push_back(sym);

	SYMBOLREF ref;
	ref.symbol = sym;
	ref.swizzle = SWIZZLE_XYZW;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::Multiply(const SYMBOLREF_MATRIX& src1, const SYMBOLREF_FLOAT4& src2)
{
	auto dst = CreateTempFloat4();
	m_statements.push_back(STATEMENT(STATEMENT_OP_MULTIPLY, SYMBOLREF(dst), SYMBOLREF(src1), SYMBOLREF(src2)));
	return dst;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::Sample(const SYMBOLREF& src1, const SYMBOLREF_FLOAT2& src2)
{
	auto result = CreateTempFloat4();

	auto statement = STATEMENT();
	statement.op			= STATEMENT_OP_SAMPLE;
	statement.dstRef		= result;
	statement.src1Ref		= src1;
	statement.src2Ref		= src2;
	m_statements.push_back(statement);

	return result;
}

CShaderBuilder::SYMBOLREF_FLOAT CShaderBuilder::Pow(const SYMBOLREF_FLOAT& src1, const SYMBOLREF_FLOAT& src2)
{
	auto dst = CreateTempFloat();
	m_statements.push_back(STATEMENT(STATEMENT_OP_POW, dst, src1, src2));
	return dst;
}

CShaderBuilder::SYMBOLREF_FLOAT CShaderBuilder::Negate(const SYMBOLREF_FLOAT& src)
{
	auto result = CreateTempFloat();

	auto statement = STATEMENT();
	statement.op			= STATEMENT_OP_NEGATE;
	statement.dstRef		= result;
	statement.src1Ref		= src;
	m_statements.push_back(statement);

	return result;
}

CShaderBuilder::SYMBOLREF_FLOAT CShaderBuilder::SwizzleFloat(const SYMBOLREF_FLOAT4& src1, SWIZZLE_TYPE swizzle)
{
	SYMBOLREF_FLOAT ref;
	ref.symbol = src1.symbol;
	ref.swizzle = swizzle;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT3 CShaderBuilder::SwizzleFloat3(const SYMBOLREF_FLOAT4& src1, SWIZZLE_TYPE swizzle)
{
	SYMBOLREF_FLOAT3 ref;
	ref.symbol = src1.symbol;
	ref.swizzle = swizzle;
	return ref;
}

CShaderBuilder::SYMBOLREF_FLOAT2 CShaderBuilder::NewFloat2(const SYMBOLREF_FLOAT& src1, const SYMBOLREF_FLOAT& src2)
{
	auto result = CreateTempFloat2();

	auto statement = STATEMENT();
	statement.op			= STATEMENT_OP_NEWVECTOR2;
	statement.dstRef		= result;
	statement.src1Ref		= src1;
	statement.src2Ref		= src2;
	m_statements.push_back(statement);

	return result;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::NewFloat4(const SYMBOLREF_FLOAT& src1, const SYMBOLREF_FLOAT& src2, const SYMBOLREF_FLOAT& src3, const SYMBOLREF_FLOAT& src4)
{
	auto result = CreateTempFloat4();

	auto statement = STATEMENT();
	statement.op			= STATEMENT_OP_NEWVECTOR4;
	statement.dstRef		= result;
	statement.src1Ref		= src1;
	statement.src2Ref		= src2;
	statement.src3Ref		= src3;
	statement.src4Ref		= src4;
	m_statements.push_back(statement);

	return result;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::NewFloat4(const SYMBOLREF_FLOAT2& src1, const SYMBOLREF_FLOAT& src2, const SYMBOLREF_FLOAT& src3)
{
	auto result = CreateTempFloat4();

	auto statement = STATEMENT();
	statement.op			= STATEMENT_OP_NEWVECTOR4;
	statement.dstRef		= result;
	statement.src1Ref		= src1;
	statement.src2Ref		= src2;
	statement.src3Ref		= src3;
	m_statements.push_back(statement);

	return result;
}

CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::NewFloat4(const SYMBOLREF_FLOAT3& src1, const SYMBOLREF_FLOAT& src2)
{
	auto result = CreateTempFloat4();

	auto statement = STATEMENT();
	statement.op			= STATEMENT_OP_NEWVECTOR4;
	statement.dstRef		= result;
	statement.src1Ref		= src1;
	statement.src2Ref		= src2;
	m_statements.push_back(statement);

	return result;
}
