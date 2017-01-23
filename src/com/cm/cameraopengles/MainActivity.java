package com.cm.cameraopengles;

import java.io.IOException;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.cm.opengles.CmOpenGLES;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

public class MainActivity extends Activity implements Camera.PreviewCallback{

	private final String TAG = MainActivity.class.getSimpleName();
	
	public final static int WIDTH = 640;
	public final static int HEIGHT = 480;
	public final static int SIZE = WIDTH * HEIGHT * 3 / 2;

	private static final boolean DEBUG = false;
	public byte[] mData;
	
	private GLSurfaceView view;
	private Camera mCamera;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		view = new GLSurfaceView(MainActivity.this);
		view.setEGLContextClientVersion(2);
		view.setRenderer(new GLSurfaceView.Renderer() {
			
			@Override
			public void onSurfaceCreated(GL10 gl, EGLConfig config) {
				openCamera(0);
				mCamera.startPreview();
				CmOpenGLES.init(WIDTH, HEIGHT);
			}
			
			@Override
			public void onSurfaceChanged(GL10 gl, int width, int height) {
				CmOpenGLES.changeLayout(width, height);
			}
			
			@Override
			public void onDrawFrame(GL10 gl) {
				if (mData != null)
				{
					CmOpenGLES.drawFrame(mData, SIZE);
				}
			}
		});
		setContentView(view);
	}
	/**
	 * ´ò¿ªÉãÏñÍ·
	 */
	public void openCamera(int id){
		mCamera = Camera.open(id);
		Parameters cp = mCamera.getParameters();
		cp.setPreviewSize(WIDTH, HEIGHT);
		cp.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
		cp.setZoom(0);
		try {
			mCamera.addCallbackBuffer(mData);
			mCamera.setPreviewCallback(MainActivity.this);
			mCamera.setPreviewTexture(new SurfaceTexture(10));
		} catch (IOException e) {
			Log.w(TAG, "setPreviewTexture error e = " + e.getMessage());
		}
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		view.onResume();
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		view.onPause();
		mCamera.stopPreview();
	}
	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		if (DEBUG)
		{
			Log.i(TAG, "onPreviewFrame()");
		}
		mData = data;
		camera.addCallbackBuffer(data);
	}
	
}
