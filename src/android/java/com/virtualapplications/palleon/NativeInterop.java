package com.virtualapplications.palleon;

public class NativeInterop
{
	 static 
	 {
		 System.loadLibrary("PalleonApplication");
	 }

	 public static native void initialize(int width, int height);
	 public static native void update();
}
