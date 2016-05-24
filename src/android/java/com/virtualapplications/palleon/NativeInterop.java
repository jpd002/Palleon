package com.virtualapplications.palleon;

import android.content.res.AssetManager;
import android.view.Surface;

public class NativeInterop
{
	 static 
	 {
		 System.loadLibrary("PalleonApplication");
	 }

	 public static native void initialize(Surface surface, int width, int height, float density);
	 public static native void update(long frameTime);
	 public static native void setAssetManager(AssetManager assetManager);
	 
	 public static native void notifyMouseMove(int x, int y);
	 public static native void notifyMouseDown();
	 public static native void notifyMouseUp();
}
