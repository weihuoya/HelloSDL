//
//  GLKMathUtils.h
//  GLKMath
//
//  Copyright (c) 2011, Apple Inc. All rights reserved.
//

#ifndef __GLK_MATH_UTILS_H
#define __GLK_MATH_UTILS_H

#include <math.h>
#include <stdbool.h>

#include <GLKMath/GLKMathTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

static __inline__ float GLKMathDegreesToRadians(float degrees) { return degrees * (M_PI / 180); };
static __inline__ float GLKMathRadiansToDegrees(float radians) { return radians * (180 / M_PI); };

#ifdef __cplusplus
}
#endif

#endif /* __GLK_MATH_UTILS_H */
