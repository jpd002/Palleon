#include <jni.h>
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

void CAndroidActivity::Initialize(int width, int height, float density)
{
	assert(!m_application);
	
	CAndroidLog::CreateInstance();
	CAndroidResourceManager::CreateInstance();
	CAndroidGraphicDevice::CreateInstance(width, height, density);

	m_application = CreateApplication();
}

void CAndroidActivity::Update()
{
	m_application->Update(1.f / 60.f);
	CAndroidGraphicDevice::GetInstance().Draw();
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

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_initialize(JNIEnv* env, jobject obj, jint width, jint height, jfloat density)
{
	CAndroidActivity::GetInstance().Initialize(width, height, density);
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_update(JNIEnv* env, jobject obj)
{
	CAndroidActivity::GetInstance().Update();
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
