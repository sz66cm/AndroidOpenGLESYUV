#include <jni.h>
#include "opengles/esUtil.h"
#include "opengles/matrix.h"
#include "cameraShader.h"


#define UNIT 1
#define TEXTURE_COOR_UNIT 1

#define NELEM(m) (sizeof(m) / sizeof((m)[0]))

//顶点着色器脚本代码
const char * codeVertexShader = \
"attribute vec3 aPosition;							\n" \
"uniform mat4 uMVPMatrix;	 						\n" \
"attribute vec2 aTexCoor; 							\n" \
"varying vec2 vTexCoor;		 						\n" \
"void main() 										\n" \
"{ 													\n" \
"	gl_Position = uMVPMatrix * vec4(aPosition, 1); 	\n" \
" 	vTexCoor = aTexCoor;							\n" \
"} 													\n" \
;

//-------------MATH---------------
const char * codeFragShader = \
"precision mediump float;											\n" \
"uniform sampler2D yTexture; 										\n" \
"uniform sampler2D uTexture; 										\n" \
"uniform sampler2D vTexture; 										\n" \
"varying vec2 vTexCoor;												\n" \
"void main()														\n" \
"{																	\n" \
"	float y = texture2D(yTexture, vTexCoor).r;						\n" \
"	float u = texture2D(uTexture, vTexCoor).r;											\n" \
"	float v = texture2D(vTexture, vTexCoor).r;													\n" \
"	vec3 yuv = vec3(y, u, v);												\n" \
"	vec3 offset = vec3(16.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0);								\n" \
"	mat3 mtr = mat3(1.0, 1.0, 1.0, -0.001, -0.3441, 1.772, 1.402, -0.7141, 0.001);						\n" \
"	vec4 curColor = vec4(mtr * (yuv - offset), 1);												\n" \
"	gl_FragColor = curColor;													\n" \
"}																	\n" \
;

//渲染顶点坐标数据
const float dataVertex[] = 
{
	-1 * UNIT, 1 * UNIT, 0,
	-1 * UNIT, -1 * UNIT, 0,
	1 * UNIT, 1 * UNIT, 0,
	1 * UNIT, -1 * UNIT, 0
};
//渲染纹理坐标数据
const float dataTexCoor[] =
{
	0 * TEXTURE_COOR_UNIT, 0 * TEXTURE_COOR_UNIT,
	0 * TEXTURE_COOR_UNIT, 1 * TEXTURE_COOR_UNIT,
	1 * TEXTURE_COOR_UNIT, 0 * TEXTURE_COOR_UNIT,
	1 * TEXTURE_COOR_UNIT, 1 * TEXTURE_COOR_UNIT
};

/*方法声明区 start*/
void printData (void* data, const int size, const char * name);
extern int registerNativeMethods(JNIEnv* env,
			     const char* className, const JNINativeMethod* gMethods, int numMethods);
/*方法声明区 end*/

//全局变量
Instance *instance;

JNIEXPORT
jint
JNI_OnLoad(JavaVM * vm, void * reserved)
{
	jint result = -1;
	LOGI_EU("JNI_OnLoad()");
	JNIEnv *env = NULL;
	instance = 0;
	result = JNI_VERSION_1_6;
	return result;
}

/**
 * OpenGL ES 2.0 初始化函数
 * @param pWidth 分辨率 宽
 * @param pHeight 分辨率 高
 * @param angle 显示图像的角度
 */
JNIEXPORT
void
Java_com_cm_opengles_CmOpenGLES_init(JNIEnv *env, jobject obj, jint pWidth, jint pHeight, jint angle)
{
	if (instance != 0)
	  {
	    release(instance);
	    instance = 0;
	  }
	LOGI_EU("init()");
	instance = (Instance *)malloc(sizeof(Instance));
	memset(instance, 0, sizeof(Instance));
	//	1.初始化着色器
	openglInit (codeVertexShader, codeFragShader, pWidth, pHeight, angle, instance);
}

JNIEXPORT
void
Java_com_cm_opengles_CmOpenGLES_changeLayout(JNIEnv *env, jobject obj, jint width, jint height)
{
	LOGI_EU("changeLayout() width = %d, height = %d", width, height);
	if(instance != 0)
	  {
	    instance->vWidth = width;
	    instance->vHeight = height;
	  }
}

/**
 * 修改显示图像的角度
 * @param angle
 */
JNIEXPORT
void
Java_com_cm_opengles_CmOpenGLES_changeAngle(JNIEnv *env, jobject obj, jint angle)
{
	LOGI_EU("changeAngle() angle = %d", angle);
	if(instance != 0)
	  {
	    instance->angle = angle;
	  }
}

JNIEXPORT
void
Java_com_cm_opengles_CmOpenGLES_drawFrame(JNIEnv *env, jobject obj, jbyteArray yuvDatas, jint size)
{
	jbyte * srcp = (*env)->GetByteArrayElements(env, yuvDatas, 0);
	memcpy(instance->yBuffer, srcp, instance->yBufferSize);
	int i,j;
	j = 0;
	for(i = instance->yBufferSize; i < size; i+=2)
	  {
	    instance->vBuffer[j] = srcp[i];
	    instance->uBuffer[j] = srcp[i + 1];
	    ++j;
	  }
	(*env)->ReleaseByteArrayElements(env, yuvDatas, srcp, JNI_ABORT);
	drawFrame(instance);
}

JNIEXPORT
void
Java_com_cm_opengles_CmOpenGLES_release(JNIEnv *env, jobject obj)
{
	LOGI_EU("release()");
	release(instance);
	instance = 0;
}

EGLint majorVersion = -1;
EGLint minorVersion = -1;
EGLDisplay display = EGL_NO_DISPLAY;
EGLSurface eglSurface = NULL;
EGLContext eglContext = NULL;

/**
 *	初始化EGL SurfaceView使用需要
 *	@param surface
 **/
JNIEXPORT
void
Java_com_cm_opengles_CmOpenGLES_initEGL(JNIEnv *env, jobject obj, jobject surface, jint width, jint height)
{
  //初始化EGL
  //eglGetDisplay
  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (display == EGL_NO_DISPLAY) {
      LOGI_EU("display = EGL_NO_DISPLAY !");
  }
  //eglInitialize
  eglInitialize(display, &majorVersion, &minorVersion);
  LOGI_EU("majorVersion = %d, minorVersion = %d!", majorVersion, minorVersion);
  //eglChooseConfig
  const EGLint attribs[] = {	
  EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
  EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
  EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, // color components - 8 bit
  EGL_NONE };
  EGLConfig configs;
  EGLint numOfConfigs;
  eglChooseConfig(display, attribs, &configs, 1, &numOfConfigs);
  EGLNativeWindowType window = NULL;
  window = ANativeWindow_fromSurface(env, surface);
  if (window == NULL) {
      LOGI_EU("window = NULL !");
  }
  ANativeWindow_setBuffersGeometry(window, width, height, 0);
  //eglCreateWindowSurface
  eglSurface = eglCreateWindowSurface(display, configs, window, NULL);
  if (eglSurface == NULL) {
      LOGI_EU("eglSurface = NULL !");
  }
  //eglCreateContext
  EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
  eglContext = eglCreateContext(display, configs, NULL, contextAttribs);
  if (eglContext == NULL) {
      LOGI_EU("eglContext = NULL !");
  }
  eglMakeCurrent(display, eglSurface, eglSurface, eglContext);
}

/**
 * 刷新EGL前后画布，界面呈现缓存图像。
 */
JNIEXPORT
void
Java_com_cm_opengles_CmOpenGLES_swapEGL(JNIEnv *env, jobject obj)
{
  //刷新EGL前后画布，界面呈现缓存图像
  //OpenGL Drawing
  eglSwapBuffers(display, eglSurface);
}

/**
 * 销毁EGL （SurfaceView使用需要）
 */
JNIEXPORT
void
Java_com_cm_opengles_CmOpenGLES_destoryEGL(JNIEnv *env, jobject obj)
{
  //销毁EGL
	  if (display != EGL_NO_DISPLAY)
	{
	  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	  if (eglContext!= EGL_NO_CONTEXT){
	    eglDestroyContext(display, eglContext);
	  }
	  if (eglContext != EGL_NO_SURFACE){
	    eglDestroySurface(display, eglSurface);
	  }
	  eglTerminate(display);
	}
	display = EGL_NO_DISPLAY;
	eglContext = EGL_NO_CONTEXT;
	eglSurface = EGL_NO_SURFACE;
}




void printData(void* data, const int size, const char * name)
{
	char * dt = (char *)data;
	int i,j;
	char * str = (char *) malloc(size * sizeof(char) * 2);
	memset(str, '\0', size * sizeof(char) * 2);
	for (i = 0; i < size; i++)
		{
			sprintf(str + 2 * i,"%02x", dt[i]);
		}
	LOGI_EU("%s , %s", name, str);
	free(str);
}
