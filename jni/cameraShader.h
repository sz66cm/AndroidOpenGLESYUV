
#ifndef _CM_CAMAERA_SHADER_
#define _CM_CAMAERA_SHADER_

#define DEBUG 0

#include "opengles/esUtil.h"
#include "opengles/matrix.h"

void
openglInit (const char* codeVertexShader, const char* codeFragShader,
	    unsigned int pWidth, unsigned int pHeight, unsigned int angle, Instance* instance);

void
changeViewPort (unsigned int height, unsigned int width, Instance* instance, unsigned int angle);

void
drawFrame(void* ins);

void
bindTexture(GLenum texture_n, unsigned int texture_id, GLsizei width, GLsizei height, const void * buffer);
/**
 * ÊÍ·Åº¯Êý
 */
void
release (Instance* instance);

#endif

