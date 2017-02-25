LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := CameraOpenGLES
LOCAL_SRC_FILES := CameraOpenGLES.c \
					./opengles/esUtil.c \
					./opengles/matrix.c \
					cameraShader.c 

LOCAL_LDLIBS += -lGLESv2 \
				-lEGL \
				-landroid \
				-llog
LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES
include $(BUILD_SHARED_LIBRARY)
