#include "Dx11ShaderGenerator.h"
#include "string_format.h"

CDx11ShaderGenerator::CDx11ShaderGenerator(const CShaderBuilder& shaderBuilder)
: m_shaderBuilder(shaderBuilder)
{

}

std::string CDx11ShaderGenerator::Generate(const std::string& methodName, const CShaderBuilder& shaderBuilder)
{
	CDx11ShaderGenerator generator(shaderBuilder);
	return generator.Generate(methodName);
}

std::string CDx11ShaderGenerator::Generate(const std::string& methodName) const
{
	std::string result;

	result += GenerateInputStruct();
	result += GenerateOutputStruct();
	result += GenerateConstants();

	//Generate samplers/textures
	{
		for(const auto& symbol : m_shaderBuilder.GetSymbols())
		{
			if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
			result += string_format("%s c_texture%d : register(t%d);\r\n",
				MakeTypeName(symbol.type).c_str(),
				symbol.index, symbol.index);
			result += string_format("SamplerState c_sampler%d : register(s%d);\r\n",
				symbol.index, symbol.index);
		}
	}

	result += string_format("OUTPUT %s(INPUT input)\r\n", methodName.c_str());
	result += "{\r\n";
	result += "\tOUTPUT output;\r\n";

	//Write all temps
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEMPORARY) continue;
		auto temporaryValue = m_shaderBuilder.GetTemporaryValue(symbol);
		result += string_format("\tfloat4 %s = float4(%f, %f, %f, %f);\r\n",
			MakeSymbolName(symbol).c_str(),
			temporaryValue.x, temporaryValue.y, temporaryValue.z, temporaryValue.w);
	}

	for(const auto& statement : m_shaderBuilder.GetStatements())
	{
		const auto& dstRef = statement.dstRef;
		const auto& src1Ref = statement.src1Ref;
		const auto& src2Ref = statement.src2Ref;
		const auto& src3Ref = statement.src3Ref;
		const auto& src4Ref = statement.src4Ref;
		switch(statement.op)
		{
		case CShaderBuilder::STATEMENT_OP_ADD:
			result += string_format("\t%s = %s + %s;\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_SUBSTRACT:
			result += string_format("\t%s = %s - %s;\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_MULTIPLY:
			if(
				(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX) || 
				(src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX)
				)
			{
				result += string_format("\t%s = mul(%s, %s);\r\n",
					PrintSymbolRef(dstRef).c_str(),
					PrintSymbolRef(src1Ref).c_str(),
					PrintSymbolRef(src2Ref).c_str());
			}
			else
			{
				result += string_format("\t%s = %s * %s;\r\n",
					PrintSymbolRef(dstRef).c_str(),
					PrintSymbolRef(src1Ref).c_str(),
					PrintSymbolRef(src2Ref).c_str());
			}
			break;
		case CShaderBuilder::STATEMENT_OP_DIVIDE:
			result += string_format("\t%s = %s / %s;\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_MAX:
			result += string_format("\t%s = max(%s, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_DOT:
			result += string_format("\t%s = dot(%s, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_POW:
			result += string_format("\t%s = pow(%s, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_MIX:
			result += string_format("\t%s = lerp(%s, %s, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str(),
				PrintSymbolRef(src3Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NEWVECTOR2:
			result += string_format("\t%s = float2(%s, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NEWVECTOR4:
			switch(statement.GetSourceCount())
			{
			case 2:
				result += string_format("\t%s = float4(%s, %s);\r\n",
					PrintSymbolRef(dstRef).c_str(),
					PrintSymbolRef(src1Ref).c_str(),
					PrintSymbolRef(src2Ref).c_str());
				break;
			case 3:
				result += string_format("\t%s = float4(%s, %s, %s);\r\n",
					PrintSymbolRef(dstRef).c_str(),
					PrintSymbolRef(src1Ref).c_str(),
					PrintSymbolRef(src2Ref).c_str(),
					PrintSymbolRef(src3Ref).c_str());
				break;
			case 4:
				result += string_format("\t%s = float4(%s, %s, %s, %s);\r\n",
					PrintSymbolRef(dstRef).c_str(),
					PrintSymbolRef(src1Ref).c_str(),
					PrintSymbolRef(src2Ref).c_str(),
					PrintSymbolRef(src3Ref).c_str(),
					PrintSymbolRef(src4Ref).c_str());
				break;
			}
			break;
		case CShaderBuilder::STATEMENT_OP_ASSIGN:
			result += string_format("\t%s = %s;\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NEGATE:
			result += string_format("\t%s = -%s;\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_SATURATE:
			result += string_format("\t%s = saturate(%s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NORMALIZE:
			result += string_format("\t%s = normalize(%s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_SAMPLE:
			result += string_format("\t%s = c_texture%d.Sample(c_sampler%d, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				src1Ref.symbol.index, src1Ref.symbol.index,
				PrintSymbolRef(src2Ref).c_str());
			break;
		default:
			assert(0);
			break;
		}
	}

	result += "\treturn output;\r\n";
	result += "}\r\n";
	return result;
}

std::string CDx11ShaderGenerator::GenerateInputStruct() const
{
	std::string result;
	result += "struct INPUT\r\n";
	result += "{\r\n";
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_INPUT) continue;
		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		result += string_format("\t%s %s : %s;\r\n",
			MakeTypeName(symbol.type).c_str(),
			MakeLocalSymbolName(symbol).c_str(), 
			MakeSemanticName(semantic).c_str());
	}
	result += "};\r\n";
	return result;
}

std::string CDx11ShaderGenerator::GenerateOutputStruct() const
{
	std::string result;
	result += "struct OUTPUT\r\n";
	result += "{\r\n";
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_OUTPUT) continue;
		auto semantic = m_shaderBuilder.GetOutputSemantic(symbol);
		result += string_format("\t%s %s : %s;\r\n",
			MakeTypeName(symbol.type).c_str(),
			MakeLocalSymbolName(symbol).c_str(),
			MakeSemanticName(semantic).c_str());
	}
	result += "};\r\n";
	return result;
}

std::string CDx11ShaderGenerator::GenerateConstants() const
{
	std::string result;
	result += "cbuffer Constants\r\n";
	result += "{\r\n";
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		auto constantType = (symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX) ? "matrix" : "float4";
		result += string_format("\t%s %s;\r\n",
			constantType, MakeLocalSymbolName(symbol).c_str());
	}
	result += "};\r\n";
	return result;
}

std::string CDx11ShaderGenerator::MakeSymbolName(const CShaderBuilder::SYMBOL& sym) const
{
	switch(sym.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_TEMPORARY:
		return string_format("t%d", sym.index);
		break;
	case CShaderBuilder::SYMBOL_LOCATION_INPUT:
		return string_format("input.%s", MakeLocalSymbolName(sym).c_str());
		break;
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
		return string_format("output.%s", MakeLocalSymbolName(sym).c_str());
		break;
	case CShaderBuilder::SYMBOL_LOCATION_UNIFORM:
		return MakeLocalSymbolName(sym);
		break;
	default:
		assert(false);
		return "unknown";
		break;
	}
}

std::string CDx11ShaderGenerator::MakeLocalSymbolName(const CShaderBuilder::SYMBOL& sym) const
{
	switch(sym.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_INPUT:
		return string_format("i%d", sym.index);
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
		return string_format("o%d", sym.index);
	case CShaderBuilder::SYMBOL_LOCATION_UNIFORM:
		return m_shaderBuilder.GetUniformName(sym);
	default:
		assert(false);
		return "unknown";
	}
}

std::string CDx11ShaderGenerator::MakeSemanticName(CShaderBuilder::SEMANTIC_INFO semantic)
{
	switch(semantic.type)
	{
	case CShaderBuilder::SEMANTIC_POSITION:
		return "POSITION";
	case CShaderBuilder::SEMANTIC_TEXCOORD:
		return string_format("TEXCOORD%d", semantic.index);
	case CShaderBuilder::SEMANTIC_SYSTEM_POSITION:
		return "SV_POSITION";
	case CShaderBuilder::SEMANTIC_SYSTEM_COLOR:
		return "SV_TARGET";
	default:
		assert(false);
		return "";
	}
}

std::string CDx11ShaderGenerator::MakeTypeName(CShaderBuilder::SYMBOL_TYPE type)
{
	switch(type)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT:
		return "float";
	case CShaderBuilder::SYMBOL_TYPE_FLOAT2:
		return "float2";
	case CShaderBuilder::SYMBOL_TYPE_FLOAT3:
		return "float3";
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		return "float4";
	case CShaderBuilder::SYMBOL_TYPE_MATRIX:
		return "matrix";
	case CShaderBuilder::SYMBOL_TYPE_TEXTURE2D:
		return "Texture2D";
	default:
		assert(false);
		return "";
	}
}

std::string CDx11ShaderGenerator::PrintSymbolRef(const CShaderBuilder::SYMBOLREF& ref) const
{
	auto symbolName = MakeSymbolName(ref.symbol);
	if(ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX)
	{
		return symbolName;
	}
	switch(ref.swizzle)
	{
	case CShaderBuilder::SWIZZLE_X:
		return symbolName + ".x";
		break;
	case CShaderBuilder::SWIZZLE_Y:
		return symbolName + ".y";
		break;
	case CShaderBuilder::SWIZZLE_W:
		return symbolName + ".w";
		break;
	case CShaderBuilder::SWIZZLE_XY:
		return symbolName + ".xy";
		break;
	case CShaderBuilder::SWIZZLE_XZ:
		return symbolName + ".xz";
		break;
	case CShaderBuilder::SWIZZLE_XYZ:
		return symbolName + ".xyz";
		break;
	case CShaderBuilder::SWIZZLE_XYZW:
		return symbolName + ".xyzw";
		break;
	default:
		assert(false);
		return symbolName;
		break;
	}
}
