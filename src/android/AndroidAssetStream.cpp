#include <cassert>
#include "palleon/android/AndroidAssetStream.h"
#include "palleon/android/AndroidActivity.h"

using namespace Palleon;

CAndroidAssetStream::CAndroidAssetStream(const char* path)
{
	auto assetManager = CAndroidActivity::GetInstance().GetAssetManager();
	m_asset = AAssetManager_open(assetManager, path, AASSET_MODE_UNKNOWN);
	assert(m_asset != nullptr);
}

CAndroidAssetStream::~CAndroidAssetStream()
{
	AAsset_close(m_asset);
}

void CAndroidAssetStream::Seek(int64 pos, Framework::STREAM_SEEK_DIRECTION whence)
{
	int whenceStd = SEEK_CUR;
	switch(whence)
	{
	case Framework::STREAM_SEEK_CUR:
		whenceStd = SEEK_CUR;
		break;
	case Framework::STREAM_SEEK_SET:
		whenceStd = SEEK_SET;
		break;
	case Framework::STREAM_SEEK_END:
		whenceStd = SEEK_END;
		break;
	default:
		assert(0);
		break;
	}
	AAsset_seek64(m_asset, pos, whenceStd);
}

uint64 CAndroidAssetStream::Tell()
{
	return AAsset_getLength64(m_asset) - AAsset_getRemainingLength64(m_asset);
}

uint64 CAndroidAssetStream::Read(void* buffer, uint64 size)
{
	int result = AAsset_read(m_asset, buffer, static_cast<size_t>(size));
	assert(result >= 0);
	return result;
}

uint64 CAndroidAssetStream::Write(const void*, uint64)
{
	assert(0);
	return 0;
}

bool CAndroidAssetStream::IsEOF()
{
	return (AAsset_getRemainingLength64(m_asset) == 0);
}

void CAndroidAssetStream::Flush()
{
	assert(0);
}
