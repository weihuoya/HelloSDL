#include "quaternion.h"


void Quaternion::setIdentity(float q[])
{
    q[0] = 0.0f;
    q[1] = 0.0f;
    q[2] = 0.0f;
    q[3] = 1.0f;
}

void Quaternion::setAngleAndAxis(float q[], float degrees, float x, float y, float z)
{
    float radians = degrees * (M_PI / 180.0f);
    float halfAngle = radians * 0.5f;
    float scale = sinf(halfAngle);

    q[0] = scale * x;
    q[1] = scale * y;
    q[2] = scale * z;
    q[3] = cosf(halfAngle);
}

void Quaternion::setAngles(float q[], float x, float y, float z)
{
    float rotateX[4];
    float rotateY[4];
    float rotateZ[4];
    float result1[4];
    float result2[4];

    Quaternion::setAngleAndAxis(rotateX, x, 1.0f, 0.0f, 0.0f);
    Quaternion::setAngleAndAxis(rotateY, y, 0.0f, 1.0f, 0.0f);
    Quaternion::setAngleAndAxis(rotateZ, z, 1.0f, 0.0f, 1.0f);

    Quaternion::multiply(result1, rotateX, q);
    Quaternion::multiply(result2, rotateY, result1);
    Quaternion::multiply(q, rotateZ, result2);
}

void Quaternion::add(float q[], const float lhs[], const float rhs[])
{
    q[0] = lhs[0] + rhs[0];
    q[1] = lhs[1] + rhs[1];
    q[2] = lhs[2] + rhs[2];
    q[3] = lhs[3] + rhs[3];
}

void Quaternion::subtract(float q[], const float lhs[], const float rhs[])
{
    q[0] = lhs[0] - rhs[0];
    q[1] = lhs[1] - rhs[1];
    q[2] = lhs[2] - rhs[2];
    q[3] = lhs[3] - rhs[3];
}

void Quaternion::multiply(float q[], const float lhs[], const float rhs[])
{
    q[0] = lhs[3] * rhs[0] + lhs[0] * rhs[3] + lhs[1] * rhs[2] - lhs[2] * rhs[1];
    q[1] = lhs[3] * rhs[1] + lhs[1] * rhs[3] + lhs[2] * rhs[0] - lhs[0] * rhs[2];
    q[2] = lhs[3] * rhs[2] + lhs[2] * rhs[3] + lhs[0] * rhs[1] - lhs[1] * rhs[0];
    q[3] = lhs[3] * rhs[3] - lhs[0] * rhs[0] - lhs[1] * rhs[1] - lhs[2] * rhs[2];
}

float Quaternion::length(const float q[])
{
    return sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

void Quaternion::conjugate(float out_q[], const float in_q[])
{
    if(!in_q) in_q = out_q;
    out_q[0] = -in_q[0];
    out_q[1] = -in_q[1];
    out_q[2] = -in_q[2];
    out_q[3] = +in_q[3];
}

void Quaternion::invert(float out_q[], const float in_q[])
{
    if(!in_q) in_q = out_q;
    float scale = 1.0f / (in_q[0] * in_q[0] + in_q[1] * in_q[1] + in_q[2] * in_q[2] + in_q[3] * in_q[3]);
    out_q[0] = -in_q[0] * scale;
    out_q[1] = -in_q[1] * scale;
    out_q[2] = -in_q[2] * scale;
    out_q[3] = +in_q[3] * scale;
}

void Quaternion::normalize(float out_q[], const float in_q[])
{
    if(!in_q) in_q = out_q;
    float scale = 1.0f / Quaternion::length(in_q);
    out_q[0] = in_q[0] * scale;
    out_q[1] = in_q[1] * scale;
    out_q[2] = in_q[2] * scale;
    out_q[3] = in_q[3] * scale;
}

void Quaternion::rotateVector3(float out_v3[], const float in_q[], const float in_v3[])
{
    float rotate[4] = {in_v3[0], in_v3[1], in_v3[2], 0.0f};
    float invert[4];
    float result1[4];
    float result2[4];

    Quaternion::invert(invert, in_q);
    Quaternion::multiply(result1, in_q, rotate);
    Quaternion::multiply(result2, result1, invert);

    out_v3[0] = result2[0];
    out_v3[1] = result2[1];
    out_v3[2] = result2[2];
}

void Quaternion::rotateVector4(float out_v4[], const float in_q[], const float in_v4[])
{
    Quaternion::rotateVector3(out_v4, in_q, in_v4);
    out_v4[3] = in_v4[3];
}

void Quaternion::makeMatrix(float out_m[], const float in_q[])
{
    float quaternion[4];
    Quaternion::normalize(quaternion, in_q);

    float x = quaternion[0];
    float y = quaternion[1];
    float z = quaternion[2];
    float w = quaternion[3];

    float _2x = x + x;
    float _2y = y + y;
    float _2z = z + z;
    float _2w = w + w;

    out_m[ 0] = 1.0f - _2y * y - _2z * z;
    out_m[ 1] = _2x * y + _2w * z;
    out_m[ 2] = _2x * z - _2w * y;
    out_m[ 3] = 0.0f;

    out_m[ 4] = _2x * y - _2w * z;
    out_m[ 5] = 1.0f - _2x * x - _2z * z;
    out_m[ 6] = _2y * z + _2w * x;
    out_m[ 7] = 0.0f;

    out_m[ 8] = _2x * z + _2w * y;
    out_m[ 9] = _2y * z - _2w * x;
    out_m[10] = 1.0f - _2x * x - _2y * y;
    out_m[11] = 0.0f;

    out_m[12] = 0.0f;
    out_m[13] = 0.0f;
    out_m[14] = 0.0f;
    out_m[15] = 1.0f;
}
