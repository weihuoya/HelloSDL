#include "glcube.h"
#include "sdlrwops.h"
#include "matrix.h"

#include <SDL_log.h>


typedef struct {
    float Position[3];
    float Color[4];
    float TexCoord[2];
} Vertex;



GLCube::GLCube(SDL_Window * window) : window_(window)
{
    SDL_GetCurrentDisplayMode(0, &displayMode_);
    glcontext_ = SDL_GL_CreateContext(window);

    // setup the viewport
    glViewport(0, 0, displayMode_.w, displayMode_.h);

    shaderProgram_ = 0;
    vertexBuffer_ = 0;
    indexBuffer_ = 0;
}

GLCube::~GLCube()
{
    glDeleteProgram(shaderProgram_);
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteBuffers(1, &indexBuffer_);

    SDL_GL_DeleteContext(glcontext_);
}

void GLCube::initialize()
{
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // 索引缓存
    glGenBuffers(1, &indexBuffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);

    // 顶点缓存
    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);


    // shader
    SDLRWops rwops;
    size_t filesize, readsize;
    char *fShaderStr, *vShaderStr;

    rwops.fromFile("fragment.glsl", "r");
    filesize = rwops.size();
    fShaderStr = (char*)malloc(filesize+1);
    fShaderStr[filesize] = 0;
    readsize = rwops.read(fShaderStr, filesize);
    rwops.close();

    rwops.fromFile("vertex.glsl", "r");
    filesize = rwops.size();
    vShaderStr = (char*)malloc(filesize+1);
    vShaderStr[filesize] = 0;
    readsize = rwops.read(vShaderStr, filesize);
    rwops.close();

    shaderProgram_ = loadShader(fShaderStr, vShaderStr);
    glUseProgram(shaderProgram_);

    free(fShaderStr);
    free(vShaderStr);
}


void GLCube::loadData()
{
    const GLubyte indices[] = {
        // Front
        0, 1, 2, 2, 3, 0,
        // Back
        4, 5, 6, 6, 7, 4,
        // Left
        8, 9, 10, 10, 11, 8,
        // Right
        12, 13, 14, 14, 15, 12,
        // Top
        16, 17, 18, 18, 19, 16,
        // Bottom
        20, 21, 22, 22, 23, 20
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    #define TEX_COORD_MAX   4
    const Vertex vertices[] = {
        // Front
        {{1, -1, 0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{1, 1, 0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, 1, 0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, 0}, {0, 0, 0, 1}, {0, 0}},
        // Back
        {{1, 1, -2}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{-1, -1, -2}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{1, -1, -2}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, 1, -2}, {0, 0, 0, 1}, {0, 0}},
        // Left
        {{-1, -1, 0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{-1, 1, 0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, 1, -2}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, -2}, {0, 0, 0, 1}, {0, 0}},
        // Right
        {{1, -1, -2}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{1, 1, -2}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{1, 1, 0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{1, -1, 0}, {0, 0, 0, 1}, {0, 0}},
        // Top
        {{1, 1, 0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{1, 1, -2}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, 1, -2}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, 1, 0}, {0, 0, 0, 1}, {0, 0}},
        // Bottom
        {{1, -1, -2}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{1, -1, 0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, -1, 0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, -2}, {0, 0, 0, 1}, {0, 0}}
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    GLint positionSlot = glGetAttribLocation(shaderProgram_, "a_position");
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    // color
    GLint colorSlot = glGetAttribLocation(shaderProgram_, "a_color");
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(12));

    // texcoord
    GLint texcoordSlot = glGetAttribLocation(shaderProgram_, "a_texcoord");
    glEnableVertexAttribArray(texcoordSlot);
    glVertexAttribPointer(texcoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(28));

    // texture
    GLuint tid = loadTexture("floor.png");
    GLint textureSlot = glGetUniformLocation(shaderProgram_, "u_texture");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tid);
    glUniform1i(textureSlot, 0);
}


void GLCube::loadMatrix()
{
    float ratio = 4.0f * (float) displayMode_.w / (float) displayMode_.h;

    float projectionMatrix[16];
    float viewMatrix[16];
    float mvpMatrix[16];
    //float rotateMatrix[16];

    float theta = rotate_ * M_PI / 180.0f;

    Matrix::frustumM(projectionMatrix, -1, 1, -1, 1, 3, 7);

    Matrix::setRotateM(rotateMatrix, rotate_, 0, 0, -1.0f);

    //Matrix::setLookAtM(viewMatrix, 0, 4.0f * sin(theta), 4.0f * cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    Matrix::multiplyMM(mvpMatrix, projectionMatrix, viewMatrix);

    GLint projectionSlot = glGetUniformLocation(shaderProgram_, "a_projection");
    glUniformMatrix4fv(projectionSlot, 1, 0, mvpMatrix);


    CC3GLMatrix *projection = [CC3GLMatrix matrix];
    float h = 4.0f * self.frame.size.height / self.frame.size.width;
    [projection populateFromFrustumLeft:-2 andRight:2 andBottom:-h/2 andTop:h/2 andNear:4 andFar:10];
    glUniformMatrix4fv(_projectionUniform, 1, 0, projection.glMatrix);

    CC3GLMatrix *modelView = [CC3GLMatrix matrix];
    [modelView populateFromTranslation:CC3VectorMake(sin(CACurrentMediaTime()), 0, -7)];
    _currentRotation += displayLink.duration * 90;
    [modelView rotateBy:CC3VectorMake(_currentRotation, _currentRotation, 0)];
    glUniformMatrix4fv(_modelViewUniform, 1, 0, modelView.glMatrix);



    float projection[16];
    Matrix::frustumM(projection, -1, 1, -1, 1, 4, 10);

    float modelView[16];
    float translate[16];
    float rotate[16];
    Matrix::translateM(translate, sin(0), 0, -7);
    Matrix::setRotateEulerM(rotate, translate);
}

