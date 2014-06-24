#ifndef __hellosdl__glcube__
#define __hellosdl__glcube__

#include "utool.h"


class GLCube
{
public:
    GLCube(SDL_Window * window);
    ~GLCube();

    void initialize();

    void drawFrame();

    void loadMatrix();
    void loadData();

private:
    SDL_Window * window_;
    SDL_GLContext glcontext_;
    SDL_DisplayMode displayMode_;

    GLuint shaderProgram_;
    GLuint vertexBuffer_;
    GLuint indexBuffer_;
};


#endif /* defined(__HelloSDL__glcube__) */
