//
//  GLKMatrixIdentity.c
//  broc
//
//  Created by Shu Chiun Cheah on 3/21/12.
//  Copyright (c) 2012 Shunobi. All rights reserved.
//

#include <stdio.h>
#include "GLKMath.h"

const GLKMatrix3 GLKMatrix3Identity = { 
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f 
};

const GLKMatrix4 GLKMatrix4Identity = { 
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

const GLKQuaternion GLKQuaternionIdentity = {
    0.0f, 0.0f, 0.0f, 1.0f
};