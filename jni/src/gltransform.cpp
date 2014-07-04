#include "matrix.h"
#include "gltransform.h"


// Vectors
static GLKVector3 _right = GLKVector3Make(1.0f, 0.0f, 0.0f);
static GLKVector3 _up = GLKVector3Make(0.0f, 1.0f, 0.0f);
static GLKVector3 _front = GLKVector3Make(0.0f, 0.0f, 1.0f);


GLTransform::GLTransform()
{
    // Depth
    depth = 0.0f;

    // Scale
    scaleStart_ = scaleEnd_ = 1.0f;

    // Translation
    translationStart_ = translationEnd_ = GLKVector2Make(0.0f, 0.0f);

    // Rotation
    r.x = 0.0f;
    r.y = 0.0f;
    r.z = 0.0f;

    rotationStart_ = GLKVector3Make(0.0f, 0.0f, 0.0f);

    rotationEnd_ = GLKQuaternionIdentity;
    rotationEnd_ = GLKQuaternionMultiply(GLKQuaternionMakeWithAngleAndVector3Axis(-r.x, _right), rotationEnd_);
    rotationEnd_ = GLKQuaternionMultiply(GLKQuaternionMakeWithAngleAndVector3Axis(-r.y, _up), rotationEnd_);
    rotationEnd_ = GLKQuaternionMultiply(GLKQuaternionMakeWithAngleAndVector3Axis(-r.z, _front), rotationEnd_);
}


GLTransform::~GLTransform()
{
}


void GLTransform::scale(float s)
{
    scaleEnd_ = s * scaleStart_;
}


void GLTransform::translate(GLVector2 t, float m)
{
    t = GLKVector2MultiplyScalar(t, m);

    float dx = translationEnd_.x + (t.x - translationStart_.x);
    float dy = translationEnd_.y - (t.y - translationStart_.y);

    translationEnd = GLKVector2Make(dx, dy);
    translationStart = GLKVector2Make(t.x, t.y);
}


void GLTransform::rotate(GLVector3 r, float m)
{
    float dx = r.x - rotationStart_.x;
    float dy = r.y - rotationStart_.y;
    float dz = r.z - rotationStart_.z;

    rotationStart_ = GLKVector3Make(r.x, r.y, r.z);
    rotationEnd_ = GLKQuaternionMultiply(GLKQuaternionMakeWithAngleAndVector3Axis(dx*m, _up), rotationEnd_);
    rotationEnd_ = GLKQuaternionMultiply(GLKQuaternionMakeWithAngleAndVector3Axis(dy*m, _right), rotationEnd_);
    rotationEnd_ = GLKQuaternionMultiply(GLKQuaternionMakeWithAngleAndVector3Axis(-dz, _front), rotationEnd_);
}


void GLTransform::getModelViewMatrix(GLKMatrix4& modelViewMatrix)
{
    GLKMatrix4 modelViewMatrix = GLKMatrix4Identity;
    GLKMatrix4 quaternionMatrix = GLKMatrix4MakeWithQuaternion(rotationEnd_);

    modelViewMatrix = GLKMatrix4Translate(modelViewMatrix, translationEnd_.x, translationEnd_.y, -depth_);
    modelViewMatrix = GLKMatrix4Multiply(modelViewMatrix, quaternionMatrix);
    modelViewMatrix = GLKMatrix4Scale(modelViewMatrix, scaleEnd_, scaleEnd_, scaleEnd_);
}
