#ifndef __hellosdl__utool__
#define __hellosdl__utool__

#include <SDL.h>

#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include <SDL_opengles.h>
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


#define BUFFER_OFFSET(i) ((char *)NULL + (i))


#define  CHECK_GL(...)   _checkGLError(__FILE__, __LINE__)
GLuint _checkGLError(const char * file, int line);


void logMatrix(const float * m, int n, int column = 4);


GLuint loadShader(const char *FS, const char *VS);


GLuint loadTexture(const char * imagefile);


#endif /* defined(__HelloSDL__utool__) */
