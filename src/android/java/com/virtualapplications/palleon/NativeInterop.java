package com.virtualapplications.palleon;

import android.content.res.AssetManager;

public class NativeInterop
{
	 static 
	 {
		 System.loadLibrary("PalleonApplication");
	 }

	 public static native void initialize(int width, int height, float density);
	 public static native void update();
	 public static native void setAssetManager(AssetManager assetManager);
	 
	 public static native void notifyMouseMove(int x, int y);
	 public static native void notifyMouseDown();
	 public static native void notifyMouseUp();
}
