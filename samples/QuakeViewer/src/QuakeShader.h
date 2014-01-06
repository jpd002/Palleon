#ifndef _QUAKESHADER_H_
#define _QUAKESHADER_H_

#include <string>
#include <vector>
#include <list>

enum QUAKE_SHADER_BLEND_FUNC
{
	QUAKE_SHADER_BLEND_ADD,
	QUAKE_SHADER_BLEND_FILTER,
	QUAKE_SHADER_BLEND_BLEND,
};

enum QUAKE_SHADER_TCMOD_TYPE
{
	QUAKE_SHADER_TCMOD_SCROLL,
	QUAKE_SHADER_TCMOD_SCALE,
	QUAKE_SHADER_TCMOD_ROTATE,
	QUAKE_SHADER_TCMOD_TURB,
	QUAKE_SHADER_TCMOD_STRETCH,
};

enum QUAKE_SHADER_WAVE_TYPE
{
	QUAKE_SHADER_WAVE_NONE = 0,
	QUAKE_SHADER_WAVE_SIN,
};

struct QUAKE_SHADER_TCMOD
{
	enum
	{
		MAX_PARAMS = 4
	};

	QUAKE_SHADER_TCMOD_TYPE		type;
	QUAKE_SHADER_WAVE_TYPE		waveType;
	float						params[MAX_PARAMS];
};

struct QUAKE_SHADER_PASS
{
public:
	typedef std::vector<QUAKE_SHADER_TCMOD> TcModStack;

	std::string				mapName;
	TcModStack				tcMods;
	QUAKE_SHADER_BLEND_FUNC blendFunc;
};

typedef std::vector<QUAKE_SHADER_PASS> QuakeShaderPassArray;

struct QUAKE_SHADER
{
	std::string				name;
	bool					isSky;
	QuakeShaderPassArray	passes;
};

typedef std::list<QUAKE_SHADER> QuakeShaderList;

#endif
