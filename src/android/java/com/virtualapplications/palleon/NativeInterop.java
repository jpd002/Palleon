package com.virtualapplications.palleon;

import android.content.res.AssetManager;

public class NativeInterop
{
	 static 
	 {
		 System.loadLibrary("PalleonApplication");
	 }

	 public static native void initialize(int width, int height);
	 public static native void update();
	 public static native void setAssetManager(AssetManager assetManager);
}
