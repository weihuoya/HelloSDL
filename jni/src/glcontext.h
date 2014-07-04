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

    void incRotate(float rotateX, float rotateY);
    void incScale(float scale);

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(GLContext);


    SDL_Window * window_;
    GLCube * cube_;

    SDL_GLContext glcontext_;
    SDL_DisplayMode displayMode_;

    GLuint shaderProgram_;
    GLuint textureBuffer_;

    float rotateX_;
    float rotateY_;

    float * accumulatedRotation_;
    float * currentRotationX_;
    float * currentRotationY_;
    float * temporaryMatrix_;
};


#endif /* defined(__HelloSDL__glcontext__) */
