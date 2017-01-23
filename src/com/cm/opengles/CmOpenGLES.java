package com.cm.opengles;

public class CmOpenGLES {
	
	static {
		System.loadLibrary("CameraOpenGLES");
	}
	
	/**
	 * 初始化OpenGL ES 2.0
	 */
	public static native void init(int width, int height);
	
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
}
