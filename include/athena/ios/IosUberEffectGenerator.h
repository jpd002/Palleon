#pragma once

#include <string>

enum DIFFUSE_MAP_COORD_SOURCE
{
	DIFFUSE_MAP_COORD_UV0			= 0,
	DIFFUSE_MAP_COORD_UV1			= 1,
	DIFFUSE_MAP_COORD_CUBE_POS		= 2,
	DIFFUSE_MAP_COORD_CUBE_REFLECT	= 3,
};

enum DIFFUSE_MAP_COMBINE_MODE
{
	DIFFUSE_MAP_COMBINE_MODULATE	= 0,
	DIFFUSE_MAP_COMBINE_LERP		= 1,
	DIFFUSE_MAP_COMBINE_ADD			= 2,
};

#define DECLARE_DIFFUSEMAPCAPS(i)					\
	unsigned int hasDiffuseMap##i			: 1;	\
	unsigned int diffuseMap##i##CoordSrc	: 2;	\
	unsigned int diffuseMap##i##CombineMode	: 2;

#define SET_HASDIFFUSEMAP(i, value)					\
	case i:											\
		hasDiffuseMap##i = value;					\
		break;

#define SET_DIFFUSEMAPCOORDSRC(i, value)			\
	case i :										\
		diffuseMap##i##CoordSrc = value;			\
		break;

#define SET_DIFFUSEMAPCOMBINEMODE(i, value)			\
	case i :										\
		diffuseMap##i##CombineMode = value;			\
		break;

namespace Athena
{
	class CIosUberEffectGenerator
	{
	public:
		struct EFFECTCAPS
		{
			unsigned int hasVertexColor	: 1;
			unsigned int hasShadowMap	: 1;
			
			DECLARE_DIFFUSEMAPCAPS(0)
			DECLARE_DIFFUSEMAPCAPS(1)
			DECLARE_DIFFUSEMAPCAPS(2)
			DECLARE_DIFFUSEMAPCAPS(3)
			DECLARE_DIFFUSEMAPCAPS(4)
			
			void setHasDiffuseMap(unsigned int idx, unsigned int value)
			{
				switch(idx)
				{
						SET_HASDIFFUSEMAP(0, value)
						SET_HASDIFFUSEMAP(1, value)
						SET_HASDIFFUSEMAP(2, value)
						SET_HASDIFFUSEMAP(3, value)
						SET_HASDIFFUSEMAP(4, value)
				}
			}
			
			void setDiffuseMapCoordSrc(unsigned int idx, unsigned int value)
			{
				switch(idx)
				{
						SET_DIFFUSEMAPCOORDSRC(0, value)
						SET_DIFFUSEMAPCOORDSRC(1, value)
						SET_DIFFUSEMAPCOORDSRC(2, value)
						SET_DIFFUSEMAPCOORDSRC(3, value)
						SET_DIFFUSEMAPCOORDSRC(4, value)						
				}
			}
			
			void setDiffuseMapCombineMode(unsigned int idx, unsigned int value)
			{
				switch(idx)
				{
						SET_DIFFUSEMAPCOMBINEMODE(0, value)
						SET_DIFFUSEMAPCOMBINEMODE(1, value)
						SET_DIFFUSEMAPCOMBINEMODE(2, value)
						SET_DIFFUSEMAPCOMBINEMODE(3, value)
						SET_DIFFUSEMAPCOMBINEMODE(4, value)						
				}
			}
			
			unsigned int reserved : 5;
		};
		static_assert(sizeof(EFFECTCAPS) == 4, "Size of EFFECTCAPS isn't 4.");
		
		static std::string		GenerateVertexShader(const EFFECTCAPS&);
		static std::string		GeneratePixelShader(const EFFECTCAPS&);
		
	private:
		static bool				HasCoordSrc(const EFFECTCAPS&, unsigned int);
		static std::string		GenerateDiffuseMapCoordOutput(unsigned int, DIFFUSE_MAP_COORD_SOURCE);
		static std::string		GenerateDiffuseMapCoordComputation(unsigned int, DIFFUSE_MAP_COORD_SOURCE);
		static std::string		GenerateDiffuseMapSampler(unsigned int, DIFFUSE_MAP_COORD_SOURCE);
		static std::string		GenerateDiffuseMapMatrixUniform(unsigned int);
		static std::string		GenerateDiffuseMapSampling(unsigned int, DIFFUSE_MAP_COORD_SOURCE, DIFFUSE_MAP_COMBINE_MODE);
	};
}
