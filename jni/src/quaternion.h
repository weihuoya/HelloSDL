#ifndef __hellosdl__quaternion__
#define __hellosdl__quaternion__

#include <cmath>

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif


class Quaternion
{
public:
    static void setIdentity(float q[]);
    static void setAngleAndAxis(float q[], float degrees, float x, float y, float z);
    static void setAngles(float q[], float x, float y, float z);

    static void add(float q[], const float lhs[], const float rhs[]);
    static void subtract(float q[], const float lhs[], const float rhs[]);
    static void multiply(float q[], const float lhs[], const float rhs[]);

    static float length(const float q[]);

    static void conjugate(float out_q[], const float in_q[]);
    static void invert(float out_q[], const float in_q[]);
    static void normalize(float out_q[], const float in_q[]);

    static void rotateVector3(float out_v3[], const float in_q[], const float in_v3[]);
    static void rotateVector4(float out_v4[], const float in_q[], const float in_v4[]);

    static void makeMatrix(float out_m[], const float in_q[]);
};


#endif /* defined(__HelloSDL__quaternion__) */
