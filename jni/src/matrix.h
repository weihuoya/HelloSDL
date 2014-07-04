
#ifndef __hellosdl__matrix__
#define __hellosdl__matrix__

#include <cmath>

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif


class Quaternion
{
public:
    static void setIdentityM(float q[]);
    static void setAngleAndAxis(float q[], float radians, float x, float y, float z);

    static void add(float q[], const float lhs[], const float rhs[]);
    static void Subtract(float q[], const float lhs[], const float rhs[]);
    static void Multiply(float q[], const float lhs[], const float rhs[]);

    static float length(float q[]);

    static void conjugate(float out_q[], const float in_q[])
    static void invert(float out_q[], const float in_q[]);
    static void normalize(float out_q[], const float in_q[]);

    static void rotateVector3();
    static void rotateVector4();
};


class Matrix
{
public:
    static void frustumM(float m[], float left, float right, float bottom, float top, float near, float far);

    static bool invertM(float mInv[], const float m[]);

    static float length(float x, float y, float z);

    static void multiplyMM(float result[], const float lhs[], const float rhs[]);
    static void multiplyMV(float resultVec[], const float lhsMat[], const float rhsVec[]);

    static void orthoM(float m[], float left, float right, float bottom, float top, float near, float far);

    static void perspectiveM(float m[], float fovy, float aspect, float zNear, float zFar);

    static void rotateM(float m[], float a, float x, float y, float z);
    static void rotateM(float rm[], const float m[], float a, float x, float y, float z);

    static void scaleM(float sm[], const float m[], float x, float y, float z);
    static void scaleM(float m[], float x, float y, float z);

    static void setIdentityM(float sm[]);

    static void setLookAtM(float rm[],
            float eyeX, float eyeY, float eyeZ,
            float centerX, float centerY, float centerZ,
            float upX, float upY, float upZ);

    static void setRotateEulerM(float rm[], float x, float y, float z);

    static void setRotateM(float rm[], float a, float x, float y, float z);

    static void translateM(float m[], float x, float y, float z);
    static void translateM(float tm[], const float m[], float x, float y, float z);

    static void transposeM(float mTrans[], const float m[]);
};


#endif /* defined(__HelloSDL__matrix__) */
