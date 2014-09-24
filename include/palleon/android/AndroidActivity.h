#pragma once

#include <android/asset_manager_jni.h>
#include "../Application.h"
#include "Singleton.h"

namespace Palleon
{
	class CAndroidActivity : public CSingleton<CAndroidActivity>
	{
	public:
		void					Initialize(int, int);
		void					Update();
		
		AAssetManager*			GetAssetManager() const;
		void					SetAssetManager(AAssetManager*);
		
		void					NotifyMouseMove(int, int);
		void					NotifyMouseDown();
		void					NotifyMouseUp();
		
	private:
		CApplication*			m_application = nullptr;
		AAssetManager*			m_assetManager = nullptr;
	};
}
