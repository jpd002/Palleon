package com.virtualapplications.palleon;

import android.app.Activity;

import android.content.Context;

import android.graphics.PixelFormat;

import android.os.Bundle;
import android.os.Handler;

import android.util.Log;
import android.util.DisplayMetrics;

import android.view.Choreographer;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.WindowManager;

public class MainActivity extends Activity 
{
	private static String TAG = "Palleon";
	private float _density = 1.0f;
	SurfaceView _view;
	
	@Override protected void onCreate(Bundle icicle) 
	{
		super.onCreate(icicle);
		NativeInterop.setAssetManager(getAssets());
		
		WindowManager windowManager = (WindowManager)getSystemService(Context.WINDOW_SERVICE);
		DisplayMetrics metrics = new DisplayMetrics();
		windowManager.getDefaultDisplay().getMetrics(metrics);
		_density = metrics.density;
		
		_view = new SurfaceView(this);
		_view.setOnTouchListener(new TouchListener());
		setContentView(_view);
	}
	
	@Override
	protected void onPostCreate(Bundle savedInstanceState)
	{
		super.onPostCreate(savedInstanceState);
		SurfaceHolder holder = _view.getHolder();
		holder.setFormat(PixelFormat.RGBA_8888);
		holder.addCallback(new SurfaceCallback());
	}
	
	@Override 
	protected void onPause() 
	{
		super.onPause();
		//_view.onPause();
	}
	
	@Override 
	protected void onResume() 
	{
		super.onResume();
		//_view.onResume();
	}

	private class SurfaceCallback implements SurfaceHolder.Callback
	{
		@Override 
		public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
		{
			Log.w(TAG, String.format("surfaceChanged -> format: %d, width: %d, height: %d", format, width, height));
			Surface surface = holder.getSurface();
			int scaledWidth = (int)((float)width / _density);
			int scaledHeight = (int)((float)height / _density);
			NativeInterop.initialize(surface, scaledWidth, scaledHeight, _density);
			Choreographer.getInstance().postFrameCallback(new FrameCallback());
			//_view.postOnAnimation(_updateRunnable);
		}
		
		@Override 
		public void surfaceCreated(SurfaceHolder holder)
		{
			Log.w(TAG, "surfaceCreated");
		}
		
		@Override 
		public void surfaceDestroyed(SurfaceHolder holder)
		{
			Log.w(TAG, "surfaceDestroyed");
		}
	}
	
	private class FrameCallback implements Choreographer.FrameCallback
	{		
		@Override
		public void doFrame(long frameTime)
		{
			NativeInterop.update(frameTime);
			Choreographer.getInstance().postFrameCallback(new FrameCallback());
		}
	}
		
	private class TouchListener implements View.OnTouchListener
	{
		@Override 
		public boolean onTouch(final View view, final MotionEvent event)
		{
			int action = event.getActionMasked();
			int pointerIndex = event.getActionIndex();
			int pointerId = event.getPointerId(pointerIndex);
			float x = event.getX(pointerIndex) / _density;
			float y = event.getY(pointerIndex) / _density;
			switch(action)
			{
			case MotionEvent.ACTION_DOWN:
				NativeInterop.notifyMouseMove((int)x, (int)y);
				NativeInterop.notifyMouseDown();
				break;
			case MotionEvent.ACTION_UP:
				NativeInterop.notifyMouseUp();
				break;
			case MotionEvent.ACTION_MOVE:
				NativeInterop.notifyMouseMove((int)x, (int)y);
				break;
			}
			return true;
		}
	}
}
