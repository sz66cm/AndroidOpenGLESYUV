#include "cameraShader.h"



extern const float dataVertex[];
extern const float dataTexCoor[];

extern void printData(void* data, const int size, const char * name);
void bindTexture();

void
drawFrame(void* ins)
{
	if(DEBUG)
	  {
	    LOGI_EU("%s", __FUNCTION__);
	  }
	//清除深度缓冲,颜色缓冲
	glClear(GL_COLOR_BUFFER_BIT);
	Instance * instance = (Instance *)ins;
	if (instance == 0)
	  {
	    LOGW_EU("%s Program is NULL return!", __FUNCTION__);
	    return;
	  }
	//选择着色程序
	glUseProgram(instance->pProgram);
	//设定视口
	changeViewPort (instance->vHeight, instance->vWidth, instance, instance->angle);
	//传入变换矩阵数据
	//		1.初始化旋转矩阵
	float * maMVPMatrix = getRotateM(NULL, 0, instance->angle, 0, 0, 1);
//	//		2.初始化观察矩阵
//	float * lookAtM = setLookAtM(NULL, 0,
//								0, 0, 2,//float eyeX, float eyeY, float eyeZ,
//								0, 0, 0,//float centerX, float centerY, float centerZ,
//								0, 1, 0//float upX, float upY, float upZ
//	);
//	//		3.透视投影矩阵
//	float radio = (float)instance->pHeight / (float)instance->pWidth;
//	float * projM = frustumM(NULL, 0,
//							-radio, radio,//float left, float right,
//							-1, 1,//float bottom, float top,
//							1, 4//float near, GLfloat far
//	);
//	//		3.2整合矩阵
//	matrixMM4(maMVPMatrix, lookAtM);
//	matrixMM4(maMVPMatrix, projM);
//	//		4.传矩阵数据到顶点着色器
	glUniformMatrix4fv(instance->maMVPMatrixHandle, 1, GL_FALSE, maMVPMatrix);
//	//		5.释放
	free(maMVPMatrix);
//	free(lookAtM);
//	free(projM);
	//传入顶点数据到着色器程序
	glVertexAttribPointer(instance->maPositionHandle,
						3,//GLint size X Y Z
						GL_FLOAT,//GLenum type
						GL_FALSE,//GLboolean normalized
						3 * 4,//GLsizei stride
						dataVertex//const GLvoid * ptr
	);
	//传入顶点纹理坐标
	glVertexAttribPointer(instance->maTexCoorHandle,
							2,//S T
							GL_FLOAT,//GLenum type
							GL_FALSE,//GLboolean normalized
							2 * 4,//GLsizei stride
							dataTexCoor//const GLvoid * ptr
	);
	//激活并绑定纹理
	bindTexture(GL_TEXTURE0, instance->yTexture, instance->pWidth, instance->pHeight, instance->yBuffer);
	bindTexture(GL_TEXTURE1, instance->uTexture, instance->pWidth / 2, instance->pHeight / 2, instance->uBuffer);
	bindTexture(GL_TEXTURE2, instance->vTexture, instance->pWidth / 2, instance->pHeight / 2, instance->vBuffer);
	glUniform1i(instance->myTextureHandle, 0);
	glUniform1i(instance->muTextureHandle, 1);
	glUniform1i(instance->mvTextureHandle, 2);
//	printData(instance->yBuffer, 20, "yBuffer 10 : ");
//	printData(instance->uBuffer, 20, "uBuffer 10 : ");
//	printData(instance->vBuffer, 20, "vBuffer 10 : ");
	//允许顶点数据数组
	glEnableVertexAttribArray(instance->maPositionHandle);
	glEnableVertexAttribArray(instance->maTexCoorHandle);
	//绘制纹理矩形
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
/**
 * 初始化OpenGL ES 2.0
 */
void
openglInit (const char* codeVertexShader, const char* codeFragShader,
	    unsigned int pWidth, unsigned int pHeight, unsigned int angle, Instance* instance)
{
  //	1.初始化着色器
  GLuint shaders[2] =
    { 0 };
  shaders[0] = initShader (codeVertexShader, GL_VERTEX_SHADER);
  shaders[1] = initShader (codeFragShader, GL_FRAGMENT_SHADER);
  instance->pProgram = initProgram (shaders, 2);
  instance->maMVPMatrixHandle = glGetUniformLocation (instance->pProgram,
						      "uMVPMatrix");
  instance->maPositionHandle = glGetAttribLocation (instance->pProgram,
						    "aPosition");
  instance->maTexCoorHandle = glGetAttribLocation (instance->pProgram,
						   "aTexCoor");
  instance->myTextureHandle = glGetUniformLocation (instance->pProgram,
						    "yTexture");
  instance->muTextureHandle = glGetUniformLocation (instance->pProgram,
						    "uTexture");
  instance->mvTextureHandle = glGetUniformLocation (instance->pProgram,
						    "vTexture");
  //	2.初始化纹理
  //		2.1生成纹理id
  glGenTextures (1, &instance->yTexture);
  glGenTextures (1, &instance->uTexture);
  glGenTextures (1, &instance->vTexture);
  LOGI_EU("init() yT = %d, uT = %d, vT = %d.", instance->yTexture,
	  instance->uTexture, instance->vTexture);
  LOGI_EU("%s %d error = %d", __FILE__, __LINE__, glGetError ());
  //	3.分配Yuv数据内存
  instance->yBufferSize = sizeof(char) * pWidth * pHeight;
  instance->uBufferSize = sizeof(char) * pWidth / 2 * pHeight / 2;
  instance->vBufferSize = sizeof(char) * pWidth / 2 * pHeight / 2;
  instance->yBuffer = (char*) malloc (instance->yBufferSize);
  instance->uBuffer = (char*) malloc (instance->uBufferSize);
  instance->vBuffer = (char*) malloc (instance->vBufferSize);
  memset (instance->yBuffer, 0, instance->yBufferSize);
  memset (instance->uBuffer, 0, instance->uBufferSize);
  memset (instance->vBuffer, 0, instance->vBufferSize);
  instance->pHeight = pHeight;
  instance->pWidth = pWidth;
  LOGI_EU("width = %d, height = %d", instance->pWidth, instance->pHeight);
  //	4.缓存图像角度
  instance->angle = angle;
  //清理背景
  glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
  LOGI_EU("%s %d error = %d", __FILE__, __LINE__, glGetError ());
}

/**
 * 绑定纹理集成函数
 */
void bindTexture(GLenum texture_n, GLuint texture_id, GLsizei width, GLsizei height, const void * buffer)
{
//	LOGI_EU("texture_n = %x, texture_id = %d, width = %d, height = %d", texture_n, texture_id, width, height);
	//处理纹理
	//		2.绑定纹理
	glActiveTexture(texture_n);//eg:GL_TEXTURE0
	//		1.1绑定纹理id
	glBindTexture(GL_TEXTURE_2D, texture_id);
	//		2.3设置采样模式
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//		1.2输入纹理数据
	glTexImage2D(GL_TEXTURE_2D,
				0,//GLint level
				GL_LUMINANCE,//GLint internalformat
				width,//GLsizei width
				height,// GLsizei height,
				0,//GLint border,
				GL_LUMINANCE,//GLenum format,
				GL_UNSIGNED_BYTE,//GLenum type,
				buffer//const void * pixels
	);
};

/**
 * 改变视口函数
 */
void changeViewPort (unsigned int height, unsigned int width, Instance* instance, unsigned int angle)
{
  unsigned int eW, eH;
  float vRadio, radio;
  vRadio = (float) height / width;
  if (angle == 90 || angle == 270)
    {
      radio = ((float) instance->pWidth / instance->pHeight);
    }
  else
    {
      radio = ((float) instance->pHeight / instance->pWidth);
    }
  if (vRadio < radio)
    {
      eH = instance->vHeight;
      eW = (unsigned int) (eH / radio);
    }
  else
    {
      eW = instance->vWidth;
      eH = (unsigned int) (eW * radio);
    }
  LOGI_EU(
      "changeViewPort() eW = %d, eH = %d, radio = %f, vRadio = %f, instance->pHeight = %d, instance->pWidth = %d",
      eW, eH, radio, vRadio, instance->pHeight, instance->pWidth);
  int offsetX, offsetY;
  if (eW == width)
    {
      offsetX = 0;
    }
  else
    {
      offsetX = (width - eW) / 2;
    }
  if (eH == height)
    {
      offsetY = 0;
    }
  else
    {
      offsetY = (height - eH) / 2;
    }
  glViewport (offsetX, offsetY, eW, eH);
  LOGI_EU("changeViewPort() offsetX = %d, offsetY = %d, eW = %d, eH = %d",
	  offsetX, offsetY, eW, eH);
}

/**
 * 释放
 */
void release(Instance* instance)
{
  if(instance != 0)
  		{
  			free(instance->yBuffer);
  			free(instance->uBuffer);
  			free(instance->vBuffer);
  			instance->yBuffer = 0;
  			instance->uBuffer = 0;
  			instance->vBuffer = 0;
  			free(instance);
  		}
}

