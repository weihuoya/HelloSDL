#ifndef __hellosdl__glcontext__
#define __hellosdl__glcontext__

#include "utool.h"
#include "glcube.h"


class GLContext
{
public:
    ~GLContext();

    static GLContext * instance();

    void initialize(SDL_Window * window);

    void drawFrame();

    void loadMatrix();
    void loadShader();
    void loadTexture();

    void flush();

private:
    GLContext();


    SDL_Window * window_;
    SDL_GLContext glcontext_;
    SDL_DisplayMode displayMode_;

    GLuint shaderProgram_;
    GLuint textureBuffer_;

    GLCube * cube_;
    size_t rotate_;
};


#endif /* defined(__HelloSDL__glcontext__) */
