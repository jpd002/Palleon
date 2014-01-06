#ifndef _QUAKESHADERPARSER_H_
#define _QUAKESHADERPARSER_H_

#include "QuakeShader.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <list>

class CQuakeShaderParser
{
public:
	enum TOKEN_TYPE
	{
		TOKEN_KEYWORD,
		TOKEN_LITERAL,
		TOKEN_NUMBER,
		TOKEN_BRACKET_OPEN,
		TOKEN_BRACKET_CLOSE,
		TOKEN_PARENT_OPEN,
		TOKEN_PARENT_CLOSE,
	};

	struct TOKEN
	{
		TOKEN(TOKEN_TYPE type)
			: type(type), string_value(""), number_value(0)
		{

		}

		TOKEN(TOKEN_TYPE type, float number)
			: type(type), string_value(""), number_value(number)
		{

		}

		TOKEN(TOKEN_TYPE type, const char* string)
			: type(type), string_value(string), number_value(0)
		{

		}

		TOKEN_TYPE		type;
		std::string		string_value;
		float			number_value;
	};

	typedef std::vector<TOKEN> TokenArray;

	static QuakeShaderList		ParseShaders(const char*);

protected:
	class CTokenReader
	{
	public:
		CTokenReader(const TokenArray& tokens)
			: m_tokens(tokens)
			, m_currentIdx(0)
		{
		}

		bool IsEof() const
		{
			return m_currentIdx >= m_tokens.size();
		}

		TOKEN Read()
		{
			if(IsEof())
			{
				throw std::runtime_error("No more available tokens.");
			}
			return m_tokens[m_currentIdx++];
		}

		TOKEN Peek() const
		{
			if(IsEof())
			{
				throw std::runtime_error("No more available tokens.");
			}
			return m_tokens[m_currentIdx];
		}

	private:
		const TokenArray&	m_tokens;
		unsigned int		m_currentIdx;
	};
								CQuakeShaderParser();
	virtual						~CQuakeShaderParser();

	QuakeShaderList				ParseFile(const char*);
	QUAKE_SHADER				ParseShader(CTokenReader&);
	QUAKE_SHADER_PASS			ParsePass(CTokenReader&);
	QUAKE_SHADER_BLEND_FUNC		ParseBlendFunc(CTokenReader&);
	static TokenArray			Tokenize(const char*);
};

#endif
