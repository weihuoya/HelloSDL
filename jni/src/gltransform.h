#ifndef __hellosdl__gltransform__
#define __hellosdl__gltransform__


#include "utool.h"
#include "GLKMath/GLKMath.h"


class GLTransform
{
public:
    GLTransform();
    ~GLTransform();

    void scale(float s);
    void translate(GLVector2 t, float m);
    void rotate(GLVector3 r, float m);

    void getModelViewMatrix(GLKMatrix4& modelViewMatrix);

private:
    //z-depth
    float depth_;

    //scale
    float scaleStart_;
    float scaleEnd_;

    //translation
    GLKVector2 translationStart_;
    GLKVector2 translationEnd_;

    //rotation
    GLKVector3 rotationStart_;
    GLKQuaternion rotationEnd_;
};


#endif /* defined(__HelloSDL__gltransform__) */
