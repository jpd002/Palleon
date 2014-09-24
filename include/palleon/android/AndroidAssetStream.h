#pragma once

#include "Stream.h"
#include <android/asset_manager_jni.h>

namespace Palleon
{
	class CAndroidAssetStream : public Framework::CStream
	{
	public:
								CAndroidAssetStream(const char*);
								CAndroidAssetStream(const CAndroidAssetStream&) = delete;
		virtual					~CAndroidAssetStream();

		CAndroidAssetStream&	operator =(const CAndroidAssetStream&) = delete;

		void					Seek(int64, Framework::STREAM_SEEK_DIRECTION) override;
		uint64					Tell() override;
		uint64					Read(void*, uint64) override;
		uint64					Write(const void*, uint64) override;
		bool					IsEOF() override;
		void					Flush() override;
		
	private:
		AAsset*					m_asset = nullptr;
	};
}
