#include <jni.h>
#include <cassert>
#include "palleon/android/AndroidActivity.h"
#include "palleon/android/AndroidGraphicDevice.h"
#include "palleon/android/AndroidLog.h"
#include "palleon/android/AndroidResourceManager.h"

using namespace Palleon;

static AndroidActivity g_androidActivity;

void palleon_library_link()
{

}

void AndroidActivity::Initialize(int width, int height)
{
	assert(!m_initialized);
	
	CAndroidLog::CreateInstance();
	CAndroidResourceManager::CreateInstance();
	CAndroidGraphicDevice::CreateInstance(width, height);

	m_application = CreateApplication();
}

void AndroidActivity::Update()
{
	m_application->Update(1.f / 60.f);
	CAndroidGraphicDevice::GetInstance().Draw();
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_initialize(JNIEnv* env, jobject obj, jint width, jint height)
{
	g_androidActivity.Initialize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_virtualapplications_palleon_NativeInterop_update(JNIEnv* env, jobject obj)
{
	g_androidActivity.Update();
}
