package com.cm.opengles;

import java.lang.ref.WeakReference;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.opengl.GLSurfaceView;
/**
 * 调用OpenGL es 2.0 实现GPU加速渲染
 * @author chenming
 *
 */
public class XWGLSurfaceView extends GLSurfaceView{
	static {
		System.loadLibrary("CameraOpenGLES");
	}
	/**
	 * surface销毁状态
	 */
	public static final int SURFACE_DESTORY = 0;
	/**
	 * surface创建状态
	 */
	public static final int SURFACE_CREATEED = 1;
	
	/**
	 * YUV原始数据
	 */
	private byte[] mData;
	/**
	 * 视频数据相关的参数
	 */
	private Parameter mP = new Parameter();
	/**
	 * GLSurfaceView 的状态 
	 * {@link SURFACE_DESTORY SURFACE_CREATEED}
	 */
	private int surfaceStatus;

	public XWGLSurfaceView(Context context) {
		this(context, 640, 480);
	}
	/**
	 * 构造方法
	 * @param context
	 * @param width 分辨率的宽度
	 * @param height 分辨率的高度
	 */
	public XWGLSurfaceView(Context context, final int pWidth, final int pHeight) {
		super(context);
		WeakReference<Activity> wactivity = new WeakReference<Activity>((Activity)context);
		wactivity.get().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		setEGLContextClientVersion(2);//设置EGL版本
		setRenderer(new GLSurfaceView.Renderer() {
			
			@Override
			public void onSurfaceCreated(GL10 gl, EGLConfig config) {
				surfaceStatus = SURFACE_CREATEED;
				CmOpenGLES.init(pWidth, pHeight);
			}
			
			@Override
			public void onSurfaceChanged(GL10 gl, int width, int height) {
				CmOpenGLES.changeLayout(width, height);
				mP.setvHeight(height);
				mP.setvWidth(width);
			}
			
			@Override
			public void onDrawFrame(GL10 gl) {
				if (mData != null && mP != null) {
					int size = mP.getpWidth() * mP.getpHeight() * 3 / 2;
					CmOpenGLES.drawFrame(mData, size);
				}
			}
		});
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
	}
	/**
	 * 重新设置View参数
	 * @param p
	 */
	public void resetView(Parameter p) {
		checkParameter(p);
		CmOpenGLES.release();
		CmOpenGLES.init(p.getpWidth(), p.getpHeight(), p.getAngle());
		CmOpenGLES.changeLayout(p.getvWidth(), p.getvHeight());
	}
	
	public byte[] getmData() {
		return mData;
	}



	public void setmData(byte[] mData) {
		this.mData = mData;
	}



	public Parameter getParameters() {
		return this.mP;
	}



	public void setParameters(Parameter mP) {
		if (this.mP.equals(mP))
			return;
		this.mP = mP;
	}

	/**
	 * 参数实体
	 * @author chenming
	 *
	 */
	public static class Parameter {
		
		public Parameter() {
			super();
			this.vWidth = 640;
			this.vHeight = 480;
			this.pWidth = 640;
			this.pHeight = 480;
			this.angle = 0;
		}
		
		public Parameter(int vWidth, int vHeight, int pWidth, int pHeight, int angle) {
			super();
			this.vWidth = vWidth;
			this.vHeight = vHeight;
			this.pWidth = pWidth;
			this.pHeight = pHeight;
			this.angle = angle;
		}
		/*实体*/
		private int vWidth;//视窗宽度
		private int vHeight;//视窗高度
		private int pWidth;//分辨率宽度
		private int pHeight;//分辨率高度
		private int angle;//旋转角度
		
		@Override
		public boolean equals(Object o) {
			if (o instanceof XWGLSurfaceView.Parameter) {
				Parameter other = (Parameter)o;
				if (other.getAngle() != this.angle 
						|| other.getpHeight() != this.pHeight
						|| other.getpWidth() != this.pWidth
						|| other.getvWidth() != this.vWidth
						|| other.getvHeight() != this.vHeight) {
					return false;
				}
				return true;
			} else {
				return false;
			}
		}
		
		public int getvWidth() {
			return vWidth;
		}
		public void setvWidth(int vWidth) {
			this.vWidth = vWidth;
		}
		public int getvHeight() {
			return vHeight;
		}
		public void setvHeight(int vHeight) {
			this.vHeight = vHeight;
		}
		public int getpWidth() {
			return pWidth;
		}
		public void setpWidth(int pWidth) {
			this.pWidth = pWidth;
		}
		public int getpHeight() {
			return pHeight;
		}
		public void setpHeight(int pHeight) {
			this.pHeight = pHeight;
		}

		public int getAngle() {
			return angle;
		}
		/**
		 * 设置图像旋转角度,并通知底层刷新渲染
		 * @param angle
		 */
		public void setAngle(int angle) {
			if (this.angle != angle) {
				this.angle = angle;
				CmOpenGLES.changeAngle(angle);
			}
		}
	}
	
	/**
	 * 校验参数函数
	 * @param p
	 */
	private void checkParameter(Parameter p) {
		if (p == null)
			throw new NullPointerException("resetView param can't be null!");
		if (p.getpWidth() == 0)
			throw new IllegalArgumentException("The width of resolution can't be zero");
		if (p.getpHeight() == 0)
			throw new IllegalArgumentException("The height of resolution can't be zero");
		if (p.getvHeight() == 0)
			throw new IllegalArgumentException("The height of view can't be zero");
		if (p.getvWidth() == 0)
			throw new IllegalArgumentException("The width of view can't be zero");
	}
}
