#ifndef __hellosdl__glcontext__
#define __hellosdl__glcontext__

#include "gltool.h"
#include "glcube.h"
#include "gltransform.h"


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
    void incTranslate(float x, float y);
    void incScale(float scale);

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(GLContext);

    SDL_Window * window_;
    GLCube * cube_;
    GLTransform * transform_;

    SDL_GLContext glcontext_;
    SDL_DisplayMode displayMode_;

    GLuint shaderProgram_;
    GLuint textureBuffer_;
};


#endif /* defined(__HelloSDL__glcontext__) */
