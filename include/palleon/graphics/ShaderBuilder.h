#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <cassert>
#include <string>
#include "math/Vector4.h"

namespace Palleon
{
	enum SEMANTIC
	{
		SEMANTIC_NONE,
		SEMANTIC_SYSTEM_POSITION,
		SEMANTIC_SYSTEM_COLOR,
		SEMANTIC_POSITION,
		SEMANTIC_TEXCOORD,
	};

	enum SWIZZLE_TYPE
	{
		SWIZZLE_X,
		SWIZZLE_Y,
		SWIZZLE_Z,
		SWIZZLE_W,

		SWIZZLE_XX,
		SWIZZLE_XY,
		SWIZZLE_XZ,
		SWIZZLE_XW,
		
		SWIZZLE_XYZ,

		SWIZZLE_XYZW
	};

	class CShaderBuilder
	{
	public:
		enum SYMBOL_TYPE
		{
			SYMBOL_TYPE_NULL,
			SYMBOL_TYPE_FLOAT,
			SYMBOL_TYPE_FLOAT2,
			SYMBOL_TYPE_FLOAT3,
			SYMBOL_TYPE_FLOAT4,
			SYMBOL_TYPE_MATRIX,
			SYMBOL_TYPE_TEXTURE2D,
		};

		enum SYMBOL_LOCATION
		{
			SYMBOL_LOCATION_NULL,
			SYMBOL_LOCATION_TEMPORARY,
			SYMBOL_LOCATION_INPUT,
			SYMBOL_LOCATION_OUTPUT,
			SYMBOL_LOCATION_UNIFORM,
			SYMBOL_LOCATION_TEXTURE,
		};

		struct SEMANTIC_INFO
		{
			SEMANTIC_INFO(SEMANTIC type, unsigned int index)
				: type(type), index(index)
			{
		
			}

			SEMANTIC type		= SEMANTIC_NONE;
			unsigned int index	= 0;
		};

		struct SYMBOL
		{
			SYMBOL_TYPE			type = SYMBOL_TYPE_NULL;
			SYMBOL_LOCATION		location = SYMBOL_LOCATION_NULL;
			unsigned int		index = 0;
		};

		struct SYMBOLREF_FLOAT
		{
			SYMBOLREF_FLOAT()
			{
		
			}

			SYMBOLREF_FLOAT(const SYMBOL& symbol, SWIZZLE_TYPE swizzle)
				: symbol(symbol), swizzle(swizzle)
			{
		
			}

			SYMBOL			symbol;
			SWIZZLE_TYPE	swizzle = SWIZZLE_X;
		};

		struct SYMBOLREF_FLOAT2
		{
			SYMBOL			symbol;
			SWIZZLE_TYPE	swizzle = SWIZZLE_XY;
		};

		struct SYMBOLREF_FLOAT3
		{
			SYMBOLREF_FLOAT3()
			{
		
			}

			SYMBOLREF_FLOAT3(const SYMBOL& symbol, SWIZZLE_TYPE swizzle)
				: symbol(symbol), swizzle(swizzle)
			{
		
			}

			SYMBOL			symbol;
			SWIZZLE_TYPE	swizzle = SWIZZLE_XYZ;
		};

		struct SYMBOLREF_FLOAT4
		{
			SYMBOL			symbol;
			SWIZZLE_TYPE	swizzle = SWIZZLE_XYZW;
		};

		struct SYMBOLREF_MATRIX
		{
			SYMBOL			symbol;
			SWIZZLE_TYPE	swizzle = SWIZZLE_XYZW;
		};

		struct SYMBOLREF
		{
			SYMBOL			symbol;
			SWIZZLE_TYPE	swizzle = SWIZZLE_XYZW;

			SYMBOLREF()
			{
		
			}

			template<typename SymbolRefType>
			SYMBOLREF(const SymbolRefType& rhs)
			{
				CopyFrom(rhs);
			}

			template<typename SymbolRefType>
			SYMBOLREF& operator =(const SymbolRefType& rhs)
			{
				CopyFrom(rhs);
				return (*this);
			}

		private:
			template <typename SymbolRefType>
			void CopyFrom(const SymbolRefType& rhs)
			{
				symbol = rhs.symbol;
				swizzle = rhs.swizzle;
			}
		};

		enum STATEMENT_OP
		{
			STATEMENT_OP_NOP,
			STATEMENT_OP_ADD,
			STATEMENT_OP_SUBSTRACT,
			STATEMENT_OP_MULTIPLY,
			STATEMENT_OP_DIVIDE,
			STATEMENT_OP_MAX,
			STATEMENT_OP_DOT,
			STATEMENT_OP_POW,
			STATEMENT_OP_MIX,
			STATEMENT_OP_NEWVECTOR2,
			STATEMENT_OP_NEWVECTOR4,
			STATEMENT_OP_ASSIGN,
			STATEMENT_OP_NEGATE,
			STATEMENT_OP_SATURATE,
			STATEMENT_OP_NORMALIZE,
			STATEMENT_OP_LENGTH,
			STATEMENT_OP_SAMPLE,
		};

		struct STATEMENT
		{
			STATEMENT()
			{
		
			}

			STATEMENT(STATEMENT_OP op, SYMBOLREF dstRef, SYMBOLREF src1Ref, SYMBOLREF src2Ref = SYMBOLREF(), SYMBOLREF src3Ref = SYMBOLREF())
				: op(op), dstRef(dstRef), src1Ref(src1Ref), src2Ref(src2Ref), src3Ref(src3Ref)
			{
		
			}

			STATEMENT_OP		op = STATEMENT_OP_NOP;
			SYMBOLREF			dstRef;
			SYMBOLREF			src1Ref;
			SYMBOLREF			src2Ref;
			SYMBOLREF			src3Ref;
			SYMBOLREF			src4Ref;

			unsigned int GetSourceCount() const
			{
				if(src4Ref.symbol.location != SYMBOL_LOCATION_NULL)
				{
					assert(src1Ref.symbol.location != SYMBOL_LOCATION_NULL);
					assert(src2Ref.symbol.location != SYMBOL_LOCATION_NULL);
					assert(src3Ref.symbol.location != SYMBOL_LOCATION_NULL);
					return 4;
				}
				else if(src3Ref.symbol.location != SYMBOL_LOCATION_NULL)
				{
					assert(src1Ref.symbol.location != SYMBOL_LOCATION_NULL);
					assert(src2Ref.symbol.location != SYMBOL_LOCATION_NULL);
					return 3;
				}
				else if(src2Ref.symbol.location != SYMBOL_LOCATION_NULL)
				{
					assert(src1Ref.symbol.location != SYMBOL_LOCATION_NULL);
					return 2;
				}
				else if(src1Ref.symbol.location != SYMBOL_LOCATION_NULL)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		};

		typedef std::vector<SYMBOL> SymbolArray;
		typedef std::list<STATEMENT> StatementList;

								CShaderBuilder();
		virtual					~CShaderBuilder();

		const SymbolArray&		GetSymbols() const;
		SEMANTIC_INFO			GetInputSemantic(const SYMBOL&) const;
		SEMANTIC_INFO			GetOutputSemantic(const SYMBOL&) const;
		std::string				GetUniformName(const SYMBOL&) const;
		CVector4				GetTemporaryValue(const SYMBOL&) const;

		const StatementList&	GetStatements() const;

		SYMBOLREF_FLOAT2		CreateInputFloat2(SEMANTIC, unsigned int = 0);
		SYMBOLREF_FLOAT3		CreateInputFloat3(SEMANTIC, unsigned int = 0);
		SYMBOLREF_FLOAT4		CreateInputFloat4(SEMANTIC, unsigned int = 0);

		SYMBOLREF_FLOAT2		CreateOutputFloat2(SEMANTIC, unsigned int = 0);
		SYMBOLREF_FLOAT3		CreateOutputFloat3(SEMANTIC, unsigned int = 0);
		SYMBOLREF_FLOAT4		CreateOutputFloat4(SEMANTIC, unsigned int = 0);

		SYMBOLREF_FLOAT			CreateConstant(float);
		SYMBOLREF_FLOAT2		CreateConstant(float, float);
		SYMBOLREF_FLOAT3		CreateConstant(float, float, float);
		SYMBOLREF_FLOAT4		CreateConstant(float, float, float, float);

		SYMBOLREF_FLOAT			CreateTempFloat();
		SYMBOLREF_FLOAT2		CreateTempFloat2();
		SYMBOLREF_FLOAT3		CreateTempFloat3();
		SYMBOLREF_FLOAT4		CreateTempFloat4();

		SYMBOLREF_FLOAT			CreateUniformFloat(const std::string&);
		SYMBOLREF_FLOAT3		CreateUniformFloat3(const std::string&);
		SYMBOLREF_MATRIX		CreateUniformMatrix(const std::string&);

		SYMBOLREF				CreateTexture2D(unsigned int);

		template <typename SymbolRefType> SymbolRefType CreateTemp();

		template <typename SymbolRefType>
		void Assign(const SymbolRefType& lhs, const SymbolRefType& rhs)
		{
			m_statements.push_back(STATEMENT(STATEMENT_OP_ASSIGN, lhs, rhs));
		};

		template <typename SymbolRefType>
		SymbolRefType Add(const SymbolRefType& src1, const SymbolRefType& src2)
		{
			auto dst = CreateTemp<SymbolRefType>();
			m_statements.push_back(STATEMENT(STATEMENT_OP_ADD, dst, src1, src2));
			return dst;
		};

		template <typename SymbolRefType>
		SymbolRefType Substract(const SymbolRefType& src1, const SymbolRefType& src2)
		{
			auto dst = CreateTemp<SymbolRefType>();
			m_statements.push_back(STATEMENT(STATEMENT_OP_SUBSTRACT, dst, src1, src2));
			return dst;
		};

		template <typename SymbolRefType>
		SymbolRefType Multiply(const SymbolRefType& src1, const SymbolRefType& src2)
		{
			auto dst = CreateTemp<SymbolRefType>();
			m_statements.push_back(STATEMENT(STATEMENT_OP_MULTIPLY, dst, src1, src2));
			return dst;
		};

		SYMBOLREF_FLOAT4		Multiply(const SYMBOLREF_MATRIX&, const SYMBOLREF_FLOAT4&);

		template <typename SymbolRefType>
		SymbolRefType Divide(const SymbolRefType& src1, const SymbolRefType& src2)
		{
			auto dst = CreateTemp<SymbolRefType>();
			m_statements.push_back(STATEMENT(STATEMENT_OP_DIVIDE, dst, src1, src2));
			return dst;
		};

		template <typename SymbolRefType>
		SymbolRefType Max(const SymbolRefType& src1, const SymbolRefType& src2)
		{
			auto dst = CreateTemp<SymbolRefType>();
			m_statements.push_back(STATEMENT(STATEMENT_OP_MAX, dst, src1, src2));
			return dst;
		}

		template <typename SymbolRefType>
		SYMBOLREF_FLOAT Dot(const SymbolRefType& src1, const SymbolRefType& src2)
		{
			auto dst = CreateTempFloat();
			m_statements.push_back(STATEMENT(STATEMENT_OP_DOT, dst, src1, src2));
			return dst;
		}

		SYMBOLREF_FLOAT			Pow(const SYMBOLREF_FLOAT&, const SYMBOLREF_FLOAT&);

		template <typename SymbolRefType>
		SymbolRefType Mix(const SymbolRefType& src1, const SymbolRefType& src2, const SYMBOLREF_FLOAT& src3)
		{
			auto dst = CreateTemp<SymbolRefType>();
			m_statements.push_back(STATEMENT(STATEMENT_OP_MIX, dst, src1, src2, src3));
			return dst;
		}

		SYMBOLREF_FLOAT			Negate(const SYMBOLREF_FLOAT&);

		template <typename SymbolRefType>
		SymbolRefType Saturate(const SymbolRefType& src1)
		{
			auto dst = CreateTemp<SymbolRefType>();
			m_statements.push_back(STATEMENT(STATEMENT_OP_SATURATE, dst, src1));
			return dst;
		}

		template <typename SymbolRefType>
		SymbolRefType Normalize(const SymbolRefType& src1)
		{
			auto dst = CreateTemp<SymbolRefType>();
			m_statements.push_back(STATEMENT(STATEMENT_OP_NORMALIZE, dst, src1));
			return dst;
		}

		template <typename SymbolRefType>
		SYMBOLREF_FLOAT Length(const SymbolRefType& src1)
		{
			auto dst = CreateTempFloat();
			m_statements.push_back(STATEMENT(STATEMENT_OP_LENGTH, dst, src1));
			return dst;
		}

		SYMBOLREF_FLOAT4		Sample(const SYMBOLREF&, const SYMBOLREF_FLOAT2&);

		template <typename SymbolRefType>
		SYMBOLREF_FLOAT SwizzleFloat(const SymbolRefType& src1, SWIZZLE_TYPE swizzle)
		{
			SYMBOLREF_FLOAT ref;
			ref.symbol = src1.symbol;
			ref.swizzle = swizzle;
			return ref;
		}

		template <typename SymbolRefType>
		SYMBOLREF_FLOAT2 SwizzleFloat2(const SymbolRefType& src1, SWIZZLE_TYPE swizzle)
		{
			SYMBOLREF_FLOAT2 ref;
			ref.symbol = src1.symbol;
			ref.swizzle = swizzle;
			return ref;
		}

		SYMBOLREF_FLOAT3		SwizzleFloat3(const SYMBOLREF_FLOAT4&, SWIZZLE_TYPE);

		SYMBOLREF_FLOAT2		NewFloat2(const SYMBOLREF_FLOAT&, const SYMBOLREF_FLOAT&);
		SYMBOLREF_FLOAT4		NewFloat4(const SYMBOLREF_FLOAT&, const SYMBOLREF_FLOAT&, const SYMBOLREF_FLOAT&, const SYMBOLREF_FLOAT&);
		SYMBOLREF_FLOAT4		NewFloat4(const SYMBOLREF_FLOAT2&, const SYMBOLREF_FLOAT&, const SYMBOLREF_FLOAT&);
		SYMBOLREF_FLOAT4		NewFloat4(const SYMBOLREF_FLOAT3&, const SYMBOLREF_FLOAT&);

	private:
		typedef std::unordered_map<unsigned int, SEMANTIC_INFO> SemanticMap;
		typedef std::unordered_map<unsigned int, std::string> UniformNameMap;
		typedef std::unordered_map<unsigned int, CVector4> TemporaryValueMap;

		SymbolArray				m_symbols;
		StatementList			m_statements;
		unsigned int			m_currentTempIndex = 0;
		unsigned int			m_currentInputIndex = 0;
		unsigned int			m_currentOutputIndex = 0;

		SemanticMap				m_inputSemantics;
		SemanticMap				m_outputSemantics;
		UniformNameMap			m_uniformNames;
		TemporaryValueMap		m_temporaryValues;
	};

	template<>
	inline CShaderBuilder::SYMBOLREF_FLOAT CShaderBuilder::CreateTemp<CShaderBuilder::SYMBOLREF_FLOAT>()
	{
		return CreateTempFloat();
	}

	template<>
	inline CShaderBuilder::SYMBOLREF_FLOAT2 CShaderBuilder::CreateTemp<CShaderBuilder::SYMBOLREF_FLOAT2>()
	{
		return CreateTempFloat2();
	}

	template<>
	inline CShaderBuilder::SYMBOLREF_FLOAT3 CShaderBuilder::CreateTemp<CShaderBuilder::SYMBOLREF_FLOAT3>()
	{
		return CreateTempFloat3();
	}

	template<>
	inline CShaderBuilder::SYMBOLREF_FLOAT4 CShaderBuilder::CreateTemp<CShaderBuilder::SYMBOLREF_FLOAT4>()
	{
		return CreateTempFloat4();
	}

	template<> 
	inline void CShaderBuilder::Assign<CShaderBuilder::SYMBOLREF>(const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&)
	{
	//	static_assert(false, "Cannot assign to generic symbol reference.");
	}

}
