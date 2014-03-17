#pragma once

#include "Texture.h"
#include "Stream.h"

namespace Athena
{
	class CTextureLoader
	{
	public:
		static TexturePtr				CreateTextureFromStream(Framework::CStream&);
		static TexturePtr				CreateTextureFromFile(const std::string&);
		static TexturePtr				CreateTextureFromMemory(const void*, uint32);

		static TexturePtr				CreateCubeTextureFromStream(Framework::CStream&);
		static TexturePtr				CreateCubeTextureFromFile(const std::string&);

	private:
		static TexturePtr				CreateCubeTextureFromStreamDds(Framework::CStream&);
		static TexturePtr				CreateCubeTextureFromStreamPvr(Framework::CStream&);
	};
}
