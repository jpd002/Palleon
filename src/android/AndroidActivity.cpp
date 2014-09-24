#include <jni.h>
#include <cassert>
#include "palleon/android/AndroidActivity.h"
#include "palleon/android/AndroidGraphicDevice.h"
#include "palleon/android/AndroidLog.h"
#include "palleon/android/AndroidResourceManager.h"

using namespace Palleon;

void palleon_library_link()
{

}

void CAndroidActivity::Initialize(int width, int height)
{
	assert(!m_initialized);
	
	CAndroidLog::CreateInstance();
	CAndroidResourceManager::CreateInstance();
	CAndroidGraphicDevice::CreateInstance(width, height);

	m_application = CreateApplication();
}

void CAndroidActivity::Update()
{
	m_application->Update(1.f / 60.f);
	CAndroidGraphicDevice::GetInstance().Draw();
}

AAssetManager* CAndroidActivity::GetAssetManager() const
{
	return m_assetManager;
}

void CAndroidActivity::SetAssetManager(AAssetManager* assetManager)
{
	m_assetManager = assetManager;
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_initialize(JNIEnv* env, jobject obj, jint width, jint height)
{
	CAndroidActivity::GetInstance().Initialize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_update(JNIEnv* env, jobject obj)
{
	CAndroidActivity::GetInstance().Update();
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_setAssetManager(JNIEnv* env, jobject obj, jobject assetManagerJava)
{
	auto assetManager = AAssetManager_fromJava(env, assetManagerJava);
	assert(assetManager != nullptr);
	CAndroidActivity::GetInstance().SetAssetManager(assetManager);
}
