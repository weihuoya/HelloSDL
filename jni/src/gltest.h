#ifndef __hellosdl__gltest__
#define __hellosdl__gltest__

#include <SDL.h>


#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include <SDL_opengles.h>
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


class GLTest
{
public:
    GLTest(SDL_Window * window);
    ~GLTest();

    void initialize();

    void flush();

    void log(const float * m, int n);

    void loadMatrix();
    void loadData();

    GLuint loadShader(const char *fs, const char *vs);
    GLuint compileShader(const char * source, int type);
    void linkShader(GLuint prog);

    void drawFrame();

private:
    SDL_Window * window_;
    SDL_GLContext glcontext_;
    SDL_DisplayMode displayMode_;

    GLuint shaderProgram_;

    GLuint vertexBuffer_;
    GLuint indexBuffer_;

    GLuint depthBuffer_;

    int rotate_;
};


#endif /* defined(__HelloSDL__gltest__) */
