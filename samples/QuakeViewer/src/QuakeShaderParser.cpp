#include <assert.h>
#include "QuakeShaderParser.h"
#include "stricmp.h"

CQuakeShaderParser::CQuakeShaderParser()
{

}

CQuakeShaderParser::~CQuakeShaderParser()
{

}

QuakeShaderList CQuakeShaderParser::ParseShaders(const char* shaderFileContents)
{
	CQuakeShaderParser parser;
	return parser.ParseFile(shaderFileContents);
}

QuakeShaderList CQuakeShaderParser::ParseFile(const char* shaderFileContents)
{
	TokenArray tokens = Tokenize(shaderFileContents);
	CTokenReader tokenReader(tokens);

	QuakeShaderList result;

	try
	{
		while(!tokenReader.IsEof())
		{
			TOKEN shaderNameToken = tokenReader.Read();
			if(shaderNameToken.type != TOKEN_LITERAL)
			{
				throw std::runtime_error("Unexpected token when expecting opening bracket.");
				break;
			}

			//Check if we got an opening bracket
			TOKEN openBracketToken = tokenReader.Read();
			if(openBracketToken.type != TOKEN_BRACKET_OPEN)
			{
				throw std::runtime_error("Unexpected token when expecting opening bracket.");
				break;
			}

			//Ok, parse shader
			QUAKE_SHADER shader = ParseShader(tokenReader);
			shader.name = shaderNameToken.string_value;

			result.push_back(shader);
		}
	}
	catch(const std::exception& exception)
	{
		assert(0);
	}

	return result;
}

QUAKE_SHADER CQuakeShaderParser::ParseShader(CTokenReader& tokenReader)
{
	QUAKE_SHADER result;
	result.isSky = false;
	while(!tokenReader.IsEof())
	{
		TOKEN currentToken = tokenReader.Read();
		if(currentToken.type == TOKEN_BRACKET_CLOSE)
		{
			//Ok, we're done here
			break;
		}
		else if(currentToken.type == TOKEN_BRACKET_OPEN)
		{
			QUAKE_SHADER_PASS pass = ParsePass(tokenReader);
			result.passes.push_back(pass);
		}
		else if(currentToken.type == TOKEN_KEYWORD)
		{
			if(!stricmp(currentToken.string_value.c_str(), "skyparms"))
			{
				result.isSky = true;
			}
		}
	}
	return result;
}

QUAKE_SHADER_PASS CQuakeShaderParser::ParsePass(CTokenReader& tokenReader)
{
	QUAKE_SHADER_PASS result;
	result.blendFunc = QUAKE_SHADER_BLEND_FILTER;
	while(!tokenReader.IsEof())
	{
		TOKEN currentToken = tokenReader.Read();
		if(currentToken.type == TOKEN_BRACKET_CLOSE)
		{
			//Ok, we're done here
			break;
		}
		else if(currentToken.type == TOKEN_BRACKET_OPEN)
		{
			throw std::runtime_error("Unexpected opening bracket while parsing pass.");
		}
		else if(currentToken.type == TOKEN_KEYWORD)
		{
			if(!stricmp(currentToken.string_value.c_str(), "map"))
			{
				TOKEN param0Token = tokenReader.Read();
				result.mapName = param0Token.string_value;
			}
			else if(!stricmp(currentToken.string_value.c_str(), "blendfunc"))
			{
				result.blendFunc = ParseBlendFunc(tokenReader);
			}
			else if(!stricmp(currentToken.string_value.c_str(), "tcmod"))
			{
				TOKEN typeToken = tokenReader.Read();
				unsigned int paramCount = 0;
				QUAKE_SHADER_TCMOD tcMod;
				memset(&tcMod, 0, sizeof(QUAKE_SHADER_TCMOD));

				if(!stricmp(typeToken.string_value.c_str(), "scroll"))
				{
					tcMod.type = QUAKE_SHADER_TCMOD_SCROLL;
					paramCount = 2;
				}
				else if(!stricmp(typeToken.string_value.c_str(), "scale"))
				{
					tcMod.type = QUAKE_SHADER_TCMOD_SCALE;
					paramCount = 2;
				}
				else if(!stricmp(typeToken.string_value.c_str(), "rotate"))
				{
					tcMod.type = QUAKE_SHADER_TCMOD_ROTATE;
					paramCount = 1;
				}
				else if(!stricmp(typeToken.string_value.c_str(), "turb"))
				{
					tcMod.type = QUAKE_SHADER_TCMOD_TURB;
					paramCount = 4;
				}
				else if(!stricmp(typeToken.string_value.c_str(), "stretch"))
				{
					tcMod.type = QUAKE_SHADER_TCMOD_STRETCH;
					paramCount = 4;

					TOKEN functionToken = tokenReader.Read();
					assert(functionToken.type == TOKEN_LITERAL);

					if(!stricmp(functionToken.string_value.c_str(), "sin"))
					{
						tcMod.waveType = QUAKE_SHADER_WAVE_SIN;
					}
				}

				if(paramCount != 0)
				{
					paramCount = std::min<unsigned int>(paramCount, QUAKE_SHADER_TCMOD::MAX_PARAMS);
					for(unsigned int i = 0; i < paramCount; i++)
					{
						TOKEN paramToken = tokenReader.Read();
						while(paramToken.type != TOKEN_NUMBER)
						{
							paramToken = tokenReader.Read();
						}
						tcMod.params[i] = paramToken.number_value;
					}
					result.tcMods.push_back(tcMod);
				}
			}
		}
	}
	return result;
}

QUAKE_SHADER_BLEND_FUNC CQuakeShaderParser::ParseBlendFunc(CTokenReader& tokenReader)
{
	TOKEN param0Token = tokenReader.Read();
	if(param0Token.type != TOKEN_LITERAL)
	{
		throw std::runtime_error("Invalid first parameter to blendfunc.");
	}
	if(!strnicmp(param0Token.string_value.c_str(), "GL_", 3))
	{
		//GL_* / GL_* style
		TOKEN param1Token = tokenReader.Peek();
		if(param1Token.type != TOKEN_LITERAL)
		{
			//Humm, didn't work
			return QUAKE_SHADER_BLEND_FILTER;
		}
		tokenReader.Read();
		if(
			!stricmp(param0Token.string_value.c_str(), "GL_SRC_ALPHA") &&
			!stricmp(param1Token.string_value.c_str(), "GL_ONE_MINUS_SRC_ALPHA"))
		{
			return QUAKE_SHADER_BLEND_BLEND;
		}
		else if(
			!stricmp(param0Token.string_value.c_str(), "GL_DST_COLOR") &&
			!stricmp(param1Token.string_value.c_str(), "GL_ZERO"))
		{
			return QUAKE_SHADER_BLEND_FILTER;
		}
		else if(
			!stricmp(param0Token.string_value.c_str(), "GL_ZERO") &&
			!stricmp(param1Token.string_value.c_str(), "GL_SRC_COLOR"))
		{
			return QUAKE_SHADER_BLEND_FILTER;
		}
		else if(
			!stricmp(param0Token.string_value.c_str(), "GL_ONE") &&
			!stricmp(param1Token.string_value.c_str(), "GL_ONE"))
		{
			return QUAKE_SHADER_BLEND_ADD;
		}
		else
		{
			return QUAKE_SHADER_BLEND_FILTER;
		}
	}
	else
	{
		if(
			!stricmp(param0Token.string_value.c_str(), "blend"))
		{
			return QUAKE_SHADER_BLEND_BLEND;
		}
		else if(
			!stricmp(param0Token.string_value.c_str(), "filter"))
		{
			return QUAKE_SHADER_BLEND_FILTER;
		}
		else if(
			!stricmp(param0Token.string_value.c_str(), "add"))
		{
			return QUAKE_SHADER_BLEND_ADD;
		}
		else
		{
			return QUAKE_SHADER_BLEND_FILTER;
		}
	}
}

