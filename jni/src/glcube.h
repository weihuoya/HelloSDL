#ifndef __hellosdl__glcube__
#define __hellosdl__glcube__

#include "utool.h"


class GLCube
{
public:
    GLCube();
    ~GLCube();

    void init();

    void load1();
    void load2();

    void draw(GLuint shaderProgram);
private:
    GLuint * vboids_;
    GLsizei count_;
};


#endif /* defined(__HelloSDL__glcube__) */
