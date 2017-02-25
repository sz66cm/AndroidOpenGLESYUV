package com.cm.opengles;

public class CmOpenGLES {
	
	static {
		System.loadLibrary("CameraOpenGLES");
	}
	
	public static void init(int width, int height){
		init(width, height, 0);
	}
	
	/**
	 * 初始化OpenGL ES 2.0
	 */
	public static native void init(int width, int height, int angle);
	
	/**
	 * 释放
	 */
	public static native void release();
	
	/**
	 * 传入宽高
	 * @param width
	 * @param height
	 */
	public static native void changeLayout(int width, int height);
	
	/**
	 * 渲染Yuv数据
	 * @param data
	 */
	public static native void drawFrame(byte[] data, int size);
	
	/**
	 * 修改显示图像的角度
	 * @param angle
	 */
	public static native void changeAngle(int angle);
	
	/**
	 * 初始化EGL （SurfaceView使用需要）
	 * @param surface
	 */
	public static native void initEGL(android.view.Surface surface, int width, int height);
	
	/**
	 * 刷新EGL前后画布，界面呈现缓存图像。
	 */
	public static native void swapEGL();
	
	/**
	 * 销毁EGL （SurfaceView使用需要）
	 * @param surface
	 */
	public static native void destoryEGL();
	
}
