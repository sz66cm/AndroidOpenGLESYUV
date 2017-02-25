package com.cm.cameraopengles;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.cm.opengles.CmOpenGLES;

import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.Area;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.SystemClock;
import android.os.PowerManager.WakeLock;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class MainActivity extends Activity implements Camera.PreviewCallback, Camera.AutoFocusCallback{

	private final String TAG = MainActivity.class.getSimpleName();
	
	public final static int WIDTH = 1280;
	public final static int HEIGHT = 720;
	public final static int SIZE = WIDTH * HEIGHT * 3 / 2;

	private static final boolean DEBUG = false;
	public byte[] mData = new byte[SIZE];
	
	private GLSurfaceView view;
	private Camera mCamera;

	private WakeLock wlock;

	private SurfaceView surfaceView;

	private SurfaceHolder surfaceHolder;
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add(1, 1, 1, "开启闪光灯");
		menu.add(1, 2, 1, "关闭闪光灯");
		menu.add(1, 3, 1, "PICTURE对焦模式");
		menu.add(1, 4, 1, "自动对焦模式");
		menu.add(1, 6, 1, "PICTURE对焦模式近");
		menu.add(1, 5, 1, "停止预览");
		menu.add(1, 7, 1, "开始预览");
		return super.onCreateOptionsMenu(menu);
	}
	
	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		switch (item.getItemId()) {
		case 1:
		{
			Parameters p = mCamera.getParameters();
			p.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
			mCamera.setParameters(p);
		}
			break;
		case 2:
		{
			Parameters p = mCamera.getParameters();
			p.setFlashMode(Camera.Parameters.FLASH_MODE_OFF);
			mCamera.setParameters(p);
		}
			break;
		case 3:
		{
			Parameters p = mCamera.getParameters();
			ArrayList<Area> listArea = new ArrayList<Area>();
			Area area = new Area(new Rect(-1000, -1000, -500, -500), 1000);
			listArea.add(area);
			p.setFocusAreas(listArea);
			p.setMeteringAreas(listArea);
			mCamera.setParameters(p);
			mCamera.autoFocus(MainActivity.this);
		}
		case 4:
		{
			Parameters p = mCamera.getParameters();
			List<String> list = p.getSupportedFocusModes();
			Log.i(TAG, list.toString());
			int mfa = p.getMaxNumFocusAreas();
			int mma = p.getMaxNumMeteringAreas();
			Log.i(TAG, "getMaxNumFocusAreas = " + mfa + " getMaxNumMeteringAreas = " + mma);
		}
		break;
		case 6:
		{
			Parameters p = mCamera.getParameters();
			ArrayList<Area> listArea = new ArrayList<Area>();
			Random random = new Random();
			int n = random.nextInt(10);
			Area area = new Area(new Rect(-100 + n, -100 + n, 100, 100), 1);
			listArea.add(area);
			p.setFocusAreas(listArea);
			p.setFlashMode(Camera.Parameters.FOCUS_MODE_AUTO);
			p.setMeteringAreas(listArea);
			List<Area> list2 = p.getFocusAreas();
			Log.i(TAG, "rect = " + list2.get(0).rect.flattenToString() + " weight = " + list2.get(0).weight);
			mCamera.setParameters(p);
			mCamera.autoFocus(MainActivity.this);
		}
		break;
		case 5:
		{
			mCamera.stopPreview();
		}
		break;
		case 7:
		{
			mCamera.startPreview();
		}
		break;

		default:
			break;
		}
		return super.onMenuItemSelected(featureId, item);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		PowerManager pm = (PowerManager)getSystemService(Context.POWER_SERVICE);
		wlock = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP, "fb");
		wlock.acquire();
//		useGLSurfaceView();
//		setContentView(view);
		surfaceView = new SurfaceView(MainActivity.this);
		surfaceHolder = surfaceView.getHolder();
		surfaceHolder.addCallback(new SurfaceHolder.Callback() {
			
			@Override
			public void surfaceDestroyed(SurfaceHolder holder) {
				CmOpenGLES.destoryEGL();
				CmOpenGLES.release();
			}
			
			@Override
			public void surfaceCreated(SurfaceHolder holder) {
				surfaceHolder = holder;
				CmOpenGLES.initEGL(surfaceHolder.getSurface(), 0, 0);
				CmOpenGLES.init(WIDTH, HEIGHT, 0);
				openCamera(0);
			}
			
			@Override
			public void surfaceChanged(SurfaceHolder holder, int format, int width,
					int height) {
				CmOpenGLES.changeLayout(width, height);
			}
		});
		setContentView(surfaceView);
	}
	/**
	 * 使用GLSurfaceView
	 */
	private void useGLSurfaceView() {
		view = new GLSurfaceView(MainActivity.this);
		view.setEGLContextClientVersion(2);
		view.setRenderer(new GLSurfaceView.Renderer() {
			
			@Override
			public void onSurfaceCreated(GL10 gl, EGLConfig config) {
				openCamera(0);
				mCamera.startPreview();
				CmOpenGLES.init(WIDTH, HEIGHT, 270);
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
		view.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
	}
	/**
	 * 打开摄像头
	 */
	@SuppressWarnings("deprecation")
	public void openCamera(int id){
		mCamera = Camera.open(id);
		Parameters cp = mCamera.getParameters();
		cp.setPreviewSize(WIDTH, HEIGHT);
		cp.setPictureSize(2048, 1152);
//		cp.setPreviewFpsRange(25000, 25000);
		cp.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
		//该步骤没有设置,导致186终端绿线花屏,因为186终端默认1280*720采集,但是按照640*480的格式播放,所以造成BUG
		mCamera.setParameters(cp);
		mCamera.autoFocus(MainActivity.this);
		mCamera.cancelAutoFocus();
		try {
			mCamera.setPreviewCallbackWithBuffer(MainActivity.this);
			mCamera.addCallbackBuffer(mData);
			mCamera.setPreviewTexture(new SurfaceTexture(10));
		} catch (IOException e) {
			Log.w(TAG, "setPreviewTexture error e = " + e.getMessage());
		}
	}
	private int fcount = 0;
	private long startTime = 0;
	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
//		fcount++;
//		if (startTime == 0) {
//			startTime = SystemClock.elapsedRealtime();
//		}
//		if(SystemClock.elapsedRealtime() - startTime >= 1000) {
//			startTime = SystemClock.elapsedRealtime();
//			Log.i(TAG, "FrameRate fps = " + fcount);
//			fcount = 0;
//		}
//		if (DEBUG)
//		{
//			Log.i(TAG, "onPreviewFrame()");
//		}
//		view.requestRender();
		CmOpenGLES.drawFrame(data, data.length);
		CmOpenGLES.swapEGL();
		camera.addCallbackBuffer(data);
	}
	
	@Override
	protected void onResume() {
		super.onResume();
//		view.onResume();
	}
	
	@Override
	protected void onPause() {
		super.onPause();
//		view.onPause();
		mCamera.stopPreview();
		mCamera.release();
		wlock.release();
	}

	@Override
	public void onAutoFocus(boolean success, Camera camera) {
		Log.i(TAG, "onAutoFocus success = " + success);
		if (success) {
			mCamera.cancelAutoFocus();
		}
	}
	
}
