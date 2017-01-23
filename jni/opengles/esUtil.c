#include "esUtil.h"

/**
 * 初始化着色器
 */
GLint
initShader(const char * strShaderCode, unsigned int shaderType)
{
  GLint shader = 0;
  //申请shader引用
  shader = glCreateShader(shaderType);
  //	1.判断申请是否成功
  if(shader != 0)
    {
      //申请成功,准备加载shader
      glShaderSource(shader,//GLint pShader
		     1,//num of shader
		     &strShaderCode,//String of shader code
		     NULL//const GLint* length
      );
      //编译shader
      glCompileShader(shader);
      if(checkInitShader(shader) <= 0)
	{
	  shader = 0;
	  goto EXIT;
	}

    }
  else
    {
      LOGW_EU("%s glCreateShader fail shader == 0", __FUNCTION__);
    }
  EXIT:
  return shader;
}

/**
 * 检查着色器初始化情况
 */
GLint
checkInitShader(GLint pShader)
{
  //获取Shader编译情况
  GLint shaderStatu = -1;
  glGetShaderiv(pShader, GL_COMPILE_STATUS, &shaderStatu);
  if (shaderStatu != 0)
    {
      LOGI_EU("checkInitShader() shader = %d, complie is success!", shaderStatu);
    }
  else
    {
      GLint logLen = 0;
      glGetShaderiv(pShader,//GLuint shader
		    GL_INFO_LOG_LENGTH,//GLenum pname
		    &logLen
      );
      GLchar pVLog[logLen];
      glGetShaderInfoLog(pShader,
			 logLen,
			 NULL,
			 pVLog
      );
      LOGW_EU("checkInitShader() shader = %d, CompileShader fail! info : %s",
	   pShader, pVLog
      );
      glDeleteShader(pShader);//释放着色器申请的指针
    }
  return shaderStatu;
}

/**
 * 初始化渲染程序
 */
GLint
initProgram(GLuint* shaderArray, GLint size)
{
  //创建程序
  GLuint pProgram = glCreateProgram();
  //判断是否申请程序成功
  if (pProgram != 0)
    {
      int i;
      for (i = 0; i < size; ++i) {
	//向程序中加入顶点着色器
	glAttachShader (pProgram, shaderArray[i]);
	GLint error = glGetError();
	if (GL_NO_ERROR != error)
	  {
	    LOGW_EU("initProgram() AttachShader[%d] error!", i);
	  }
      }
      //连接程序
      glLinkProgram (pProgram);
      //获取链接状态
      if(checkLinkProgram(pProgram) != 0)
	{
	  LOGI_EU("initProgram() success!");
	}
      else
	{
	  pProgram = 0;
	  LOGI_EU("initProgram() glLinkProgram fail!");
	}
    }
  else
    {
      GLint error = glGetError();
      LOGW_EU("initProgram() pProgram glCreateProgram() fail! error = %d", error);
    }
  return pProgram;
}

/**
 * 检查程序链接状态
 */
GLint
checkLinkProgram (GLint pProgram)
{
  GLint linkProgramStatu = 0;
  glGetProgramiv (pProgram,
  GL_LINK_STATUS,
		  &linkProgramStatu);
  if (linkProgramStatu != GL_TRUE)
    {
      GLint logLen = 0; //存储Log长度
      glGetProgramiv (pProgram,
      GL_INFO_LOG_LENGTH,
		      &logLen);
      GLchar logStr[logLen];
      glGetProgramInfoLog (pProgram, logLen,
      NULL,
			   logStr);
      LOGI_EU("glLinkProgram fail! info : %s", logStr);
      glDeleteProgram (pProgram);
    }
  else
    {
      LOGI_EU("glLinkProgram success!");
    }
  return linkProgramStatu;
}
