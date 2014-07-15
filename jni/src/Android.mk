LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/$(SDL_PATH)/include \
    ../SDL2_image \
    ../SDL2_ttf \
    ../sigcpp2

# Add your application source files here...
LOCAL_SRC_FILES := \
    $(SDL_PATH)/src/main/android/SDL_android_main.c \
    GLKMath/GLKIdentity.c \
    main.cpp \
    eventhandler.cpp \
    glcontext.cpp \
    gltransform.cpp \
    glcube.cpp \
    gltool.cpp \
    sdlrwops.cpp \
    matrix.cpp \
    input.cpp \
    quaternion.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf sigc++

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
