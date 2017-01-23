#ifndef CM_MATRIX_H
#define CM_MATRIX_H
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define PI 3.14159265f

#define LOG_TAG_MATRIX "matrix"
#define LOGI_MATRIX(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG_MATRIX,__VA_ARGS__)
#define LOGW_MATRIX(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG_MATRIX,__VA_ARGS__)


void
printArray(char* name, float* rm);

//求点之间长度
float
length (float x, float y, float z);

//设置旋转矩阵
float*
getRotateM (float* m, int rmOffset, float a, float x, float y, float z);

//平移矩阵
void
translateM (float* m, int mOffset, float x, float y, float z);

//摄像头矩阵
float*
setLookAtM (float* rm, int rmOffset, float eyeX, float eyeY, float eyeZ,
	    float centerX, float centerY, float centerZ, float upX, float upY,
	    float upZ);

/**
 * 4x4矩陣相乘
 * param left : 左子矩陣,並且最後存放結果
 */
void
matrixMM4 (float* left, float* right);

/**
 * 透视矩阵
 *
 */
float*
frustumM (float* m, int offset, float left, float right, float bottom,
	  float top, float near, float far);

#endif
