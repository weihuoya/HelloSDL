#ifndef __hellosdl__gltransform__
#define __hellosdl__gltransform__


class GLTransform
{
public:
    GLTransform();
    ~GLTransform();

    void start();

    void scale(float s);
    void translate(float x, float y);
    void rotate(float x, float y, float z);

    void getModelViewMatrix(float modelViewMatrix[]);

private:
    //z-depth
    float depth_;

    //scale
    float scaleStart_;
    float scaleEnd_;

    //translation
    float translationStart_[2];
    float translationEnd_[2];

    //rotation
    float rotationStart_[3];
    float rotationEnd_[4];
};


#endif /* defined(__HelloSDL__gltransform__) */
