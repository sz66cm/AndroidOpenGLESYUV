/***此类用于生成一些OpenGL ES 2.0 的对象****/
#ifndef _CM_ES_UTIL_
#define _CM_ES_UTIL_
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "matrix.h"

#define LOG_TAG "esUtil"
#define LOGI_EU(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW_EU(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)

typedef struct Instance
{
  	//顶点着色器位置数据引用
	unsigned int maPositionHandle;
	//顶点着色器纹理坐标引用
	unsigned int maTexCoorHandle;
	//着色器程序引用
	unsigned int pProgram;
	//顶点着色器最终变化矩阵引用
	unsigned int maMVPMatrixHandle;
	//片元着色器采样器引用
	unsigned int myTextureHandle;
	unsigned int muTextureHandle;
	unsigned int mvTextureHandle;
	//纹理数据
	unsigned int yTexture;
	unsigned int uTexture;
	unsigned int vTexture;
	//着色器渲染宽高
	unsigned int pWidth;
	unsigned int pHeight;
	//屏幕的宽高
	unsigned int vWidth;
	unsigned int vHeight;
	//yuv数据
	signed char *yBuffer;
	unsigned long yBufferSize;
	signed char *uBuffer;
	unsigned long uBufferSize;
	signed char *vBuffer;
	unsigned long vBufferSize;
} Instance;

/**
 * 初始化着色器
 */
GLint
initShader(const char * strShaderCode, unsigned int shaderType);

/**
 * 检查着色器初始化状态
 */
GLint
checkInitShader(GLint pShader);

/**
 * 初始化渲染程序
 */
GLint
initProgram(GLuint* shaderArray, GLint size);

/**
 * 检查程序链接状态
 */
GLint
checkLinkProgram(GLint pProgram);


#endif
