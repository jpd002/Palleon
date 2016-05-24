#include <jni.h>
#include <android/native_window_jni.h>
#include <cassert>
#include "android/AssetManager.h"
#include "palleon/android/AndroidActivity.h"
#include "palleon/android/AndroidGraphicDevice.h"
#include "palleon/android/AndroidLog.h"
#include "palleon/android/AndroidResourceManager.h"

using namespace Palleon;

void palleon_library_link()
{

}

void CAndroidActivity::Initialize(ANativeWindow* nativeWindow, int width, int height, float density)
{
	assert(!m_application);
	
	CAndroidLog::CreateInstance();
	CAndroidResourceManager::CreateInstance();
	CAndroidGraphicDevice::CreateInstance(nativeWindow, width, height, density);

	m_application = CreateApplication();
}

void CAndroidActivity::Update(uint64 frameTime)
{
	float dt = 1.f / 60.f;
	if(m_lastFrameTime != 0)
	{
		dt = static_cast<float>(frameTime - m_lastFrameTime) / 1000000000.f;
	}
	m_lastFrameTime = frameTime;
	m_application->Update(dt);
	CAndroidGraphicDevice::GetInstance().Draw();
	static_cast<CAndroidGraphicDevice&>(CAndroidGraphicDevice::GetInstance()).PresentBackBuffer();
}

void CAndroidActivity::NotifyMouseMove(int x, int y)
{
	assert(m_application);
	m_application->NotifyMouseMove(x, y);
}

void CAndroidActivity::NotifyMouseDown()
{
	assert(m_application);
	m_application->NotifyMouseDown();
}

void CAndroidActivity::NotifyMouseUp()
{
	assert(m_application);
	m_application->NotifyMouseUp();
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_initialize(JNIEnv* env, jobject obj, jobject nativeSurface, jint width, jint height, jfloat density)
{
	auto nativeWindow = ANativeWindow_fromSurface(env, nativeSurface);
	CAndroidActivity::GetInstance().Initialize(nativeWindow, width, height, density);
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_update(JNIEnv* env, jobject obj, jlong frameTime)
{
	CAndroidActivity::GetInstance().Update(frameTime);
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_setAssetManager(JNIEnv* env, jobject obj, jobject assetManagerJava)
{
	auto assetManager = AAssetManager_fromJava(env, assetManagerJava);
	assert(assetManager != nullptr);
	Framework::Android::CAssetManager::GetInstance().SetAssetManager(assetManager);
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_notifyMouseMove(JNIEnv* env, jobject obj, jint x, jint y)
{
	CAndroidActivity::GetInstance().NotifyMouseMove(x, y);
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_notifyMouseDown(JNIEnv* env, jobject obj)
{
	CAndroidActivity::GetInstance().NotifyMouseDown();
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_notifyMouseUp(JNIEnv* env, jobject obj)
{
	CAndroidActivity::GetInstance().NotifyMouseUp();
}
