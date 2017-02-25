package com.cm.cameraopengles;

import java.io.IOException;

import com.cm.opengles.XWGLSurfaceView;
import com.cm.opengles.XWGLSurfaceView.Parameter;

import android.app.Activity;
import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.view.Menu;
import android.view.MenuItem;

public class SecondActivity extends Activity implements Camera.PreviewCallback{

	private WakeLock wlock;
	
	public static final int WIDTH = 352; 
	public static final int HEIGHT = 288; 
	public final byte[] mData = new byte[WIDTH * HEIGHT * 3 / 2];
	
	private Camera mCamera;
	private XWGLSurfaceView mXWView;
	
	private boolean needRender = false;
	private int mAngle = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		PowerManager pm = (PowerManager)getSystemService(Context.POWER_SERVICE);
		wlock = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP, "fb");
		wlock.acquire();
		mXWView = new XWGLSurfaceView(SecondActivity.this, WIDTH, HEIGHT);
		Parameter p = mXWView.getParameters();
		setContentView(mXWView);
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add(1, 1, 1, "开启相机");
		menu.add(1,2,1, "关闭相机");
		menu.add(1,3,1, "渲染数据");
		menu.add(1,4,1, "停止渲染数据");
		menu.add(1,5,1, "旋转");
		return super.onCreateOptionsMenu(menu);
	}
	
	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		switch (item.getItemId()) {
		case 1:
		{
			openCamera(0);
		}
			break;
		case 2:
		{
			mCamera.stopPreview();
			mCamera.release();
		}
		break;
		case 3:
		{
			mXWView.setmData(mData);
			needRender = true;
		}
		break;
		case 4:
		{
			needRender = false;
		}
		break;
		case 5:
		{
			Parameter p = mXWView.getParameters();
			p.setAngle(mAngle);
			mAngle += 45;
			mXWView.setParameters(p);
		}
		break;
		default:
			break;
		}
		return super.onMenuItemSelected(featureId, item);
	}
	/**
	 * 配置并开启摄像头采集数据
	 * @param id
	 */
	private void openCamera(int id) {
		mCamera = Camera.open(id);
		Parameters p = mCamera.getParameters();
		p.setPreviewSize(WIDTH, HEIGHT);
		mCamera.setParameters(p);
		try {
			mCamera.setPreviewTexture(new SurfaceTexture(10));
			mCamera.setPreviewCallbackWithBuffer(SecondActivity.this);
			mCamera.addCallbackBuffer(mData);
			mCamera.startPreview();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		camera.addCallbackBuffer(data);
		if (needRender && mXWView != null) {
			mXWView.requestRender();
		}
	}
	
	@Override
	protected void onResume() {
		super.onResume();
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		wlock.release();
	}
}
