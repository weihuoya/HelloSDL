
#ifndef __hellosdl__glcontext__
#define __hellosdl__glcontext__

#include <SDL.h>


#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include <SDL_opengles.h>
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


class GLContext
{
public:
    GLContext(SDL_Window * window);
    ~GLContext();

    void draw();
    void flush();

    void initialize();

    void drawQuad2D(const float * vb);
    void drawLine2D(const float * vb);
    void drawPoint2D(const float * vb);

    void drawQuad3D(const float * vb);

    void log(const float * m, int n);
    void drawTest();

    GLuint compileShader(const char * source, int type);
    void linkShader(GLuint prog);
    void loadShader(const char *fs, const char *vs);
    void unloadShader();

    void loadMatrix();
    void setupFrameBuffer();

private:
    SDL_Window * window_;
    SDL_GLContext glcontext_;
    SDL_DisplayMode displayMode_;

    GLuint shaderProgram_;

    GLuint vertexBuffer_;
    GLuint indexBuffer_;

    GLuint frameBuffer_;
    GLuint colorBuffer_;
    GLuint depthBuffer_;
    GLuint stencilBuffer_;

    GLuint texture_;
};


#endif /* defined(__HelloSDL__glcontext__) */
