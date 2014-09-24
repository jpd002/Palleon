package com.virtualapplications.palleon;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import java.io.File;

public class MainActivity extends Activity 
{
	MainView _view;

	@Override protected void onCreate(Bundle icicle) 
	{
		super.onCreate(icicle);
		NativeInterop.setAssetManager(getAssets());
		_view = new MainView(getApplication(), false, 24, 8);
		setContentView(_view);
	}

	@Override protected void onPause() 
	{
		super.onPause();
		_view.onPause();
	}

	@Override protected void onResume() 
	{
		super.onResume();
		_view.onResume();
	}
}
