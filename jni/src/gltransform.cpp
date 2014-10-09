#include "matrix.h"
#include "quaternion.h"
#include "gltransform.h"


GLTransform::GLTransform()
{
    // Depth
    depth_ = 4.0f;

    // Scale
    scaleStart_ = scaleEnd_ = 1.0f;

    // Translation
    translationStart_[0] = translationStart_[1] = 0.0f;
    translationEnd_[0] = translationEnd_[1] = 0.0f;

    // Rotation
    rotationStart_[0] = rotationStart_[1] = rotationStart_[2] = 0.0f;
    Quaternion::setIdentity(rotationEnd_);
    Quaternion::setAngles(rotationEnd_, 0.0f, 0.0f, 0.0f);
}


GLTransform::~GLTransform()
{
}


void GLTransform::start()
{
    scaleStart_ = scaleEnd_;
    translationStart_[0] = translationStart_[1] = 0.0f;
    rotationStart_[0] = rotationStart_[1] = rotationStart_[2] = 0.0f;
}


void GLTransform::scale(float s)
{
    scaleEnd_ = s * scaleStart_;
}


void GLTransform::translate(float x, float y)
{
    translationEnd_[0] = translationEnd_[0] + (x - translationStart_[0]);
    translationEnd_[1] = translationEnd_[1] - (y - translationStart_[1]);

    translationStart_[0] = x;
    translationStart_[1] = y;
}


void GLTransform::rotate(float x, float y, float z)
{
    float dx = x - rotationStart_[0];
    float dy = y - rotationStart_[1];
    float dz = z - rotationStart_[2];

    rotationStart_[0] = x;
    rotationStart_[1] = y;
    rotationStart_[2] = z;

    Quaternion::setAngles(rotationEnd_, dx, dy, dz);
}


void GLTransform::getModelViewMatrix(float modelViewMatrix[])
{
    float translateMatrix[16];
    float rotateMatrix[16];

    Matrix::setIdentityM(translateMatrix);
    Matrix::translateM(translateMatrix, translationEnd_[0], translationEnd_[1], -depth_);

    Quaternion::makeMatrix(rotateMatrix, rotationEnd_);

    Matrix::multiplyMM(modelViewMatrix, translateMatrix, rotateMatrix);

    Matrix::scaleM(modelViewMatrix, scaleEnd_, scaleEnd_, scaleEnd_);
}
