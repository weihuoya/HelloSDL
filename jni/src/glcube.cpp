#include "glcube.h"
#include "sdlrwops.h"
#include "matrix.h"

#include <SDL_log.h>


typedef struct {
    float Position[3];
    float Color[4];
    float TexCoord[2];
} Vertex;


// model
static float matrixModel[16];
// view
static float matrixView[16];
// projection
static float matrixProjection[16];
// projection * view * model
static float matrixMVP[16];



GLCube::GLCube(SDL_Window * window) : window_(window)
{
    SDL_GetCurrentDisplayMode(0, &displayMode_);
    glcontext_ = SDL_GL_CreateContext(window);

    // setup the viewport
    glViewport(0, 0, displayMode_.w, displayMode_.h);

    shaderProgram_ = 0;
    vertexBuffer_ = 0;
    indexBuffer_ = 0;

    // rotate
    rotate_ = 0;
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
    CHECK_GL();


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
    CHECK_GL();

    free(fShaderStr);
    free(vShaderStr);


    loadData2();
}


void GLCube::loadData1()
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
        {{+1, -1, +0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, +1, +0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, +1, +0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, +0}, {1, 0, 0, 1}, {0, 0}},
        // Back
        {{+1, +1, -1}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{-1, -1, -1}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{+1, -1, -1}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, +1, -1}, {1, 0, 0, 1}, {0, 0}},
        // Left
        {{-1, -1, +0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{-1, +1, +0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, +1, -1}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, -1}, {1, 0, 0, 1}, {0, 0}},
        // Right
        {{+1, -1, -1}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, +1, -1}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{+1, +1, +0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{+1, -1, +0}, {1, 0, 0, 1}, {0, 0}},
        // Top
        {{+1, +1, +0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, +1, -1}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, +1, -1}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, +1, +0}, {1, 0, 0, 1}, {0, 0}},
        // Bottom
        {{+1, -1, -1}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, -1, +0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, -1, +0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, -1}, {1, 0, 0, 1}, {0, 0}}
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
    /*GLint texcoordSlot = glGetAttribLocation(shaderProgram_, "a_texcoord");
    glEnableVertexAttribArray(texcoordSlot);
    glVertexAttribPointer(texcoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(28));*/

    // texture
    /*GLuint tid = loadTexture("floor.png");
    GLint textureSlot = glGetUniformLocation(shaderProgram_, "u_texture");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tid);
    glUniform1i(textureSlot, 0);*/
    CHECK_GL();
}


void GLCube::loadMatrix()
{
    // model
    Matrix::setIdentityM(matrixModel);
    Matrix::setRotateM(matrixModel, rotate_, 1.0f, 0.0f, 0.0f);


    // view
    // Position the eye behind the origin.
    float eyeX = 0.0f;
    float eyeY = 0.0f;
    float eyeZ = 1.5f;
    // We are looking toward the distance
    float lookX = 0.0f;
    float lookY = 0.0f;
    float lookZ = -5.0f;
    // Set our up vector. This is where our head would be pointing were we holding the camera.
    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;
    // Set the view matrix. This matrix can be said to represent the camera position.
    // NOTE: In OpenGL 1, a ModelView matrix is used, which is a combination of a model and
    // view matrix. In OpenGL 2, we can keep track of these matrices separately if we choose.
    Matrix::setLookAtM(matrixView, eyeX, eyeY, eyeZ, lookX, lookY, lookZ, upX, upY, upZ);


    // projection
    float width = displayMode_.w;
    float height= displayMode_.h;
    float ratio = width / height;

    float left = -ratio;
    float right = ratio;
    float bottom = -1.0f;
    float top = 1.0f;
    float near = 1.0f;
    float far = 10.0f;
    Matrix::frustumM(matrixProjection, left, right, bottom, top, near, far);


    // projection * view * model
    Matrix::multiplyMM(matrixMVP, matrixView, matrixModel);
    Matrix::multiplyMM(matrixMVP, matrixProjection, matrixMVP);


    // mvp
    GLint mvpSlot = glGetUniformLocation(shaderProgram_, "u_mvpmatrix");
    glUniformMatrix4fv(mvpSlot, 1, 0, matrixMVP);
    CHECK_GL();
}


void GLCube::drawFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    loadMatrix();
    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_SHORT, 0);
    flush();
    rotate_ = (rotate_ + 1) % 360;
}


void GLCube::flush()
{
    // Swap our back buffer to the front
    SDL_GL_SwapWindow(window_);
}


void GLCube::loadData2()
{
    GLushort indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 6, 5, 4, 7, 6,
        2, 7, 3, 7, 6, 2,
        0, 4, 1, 4, 1, 5,
        6, 2, 1, 1, 6, 5,
        0, 3, 7, 0, 7, 4,
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLfloat vertices[] = {
        +0.5f, -0.5f, +0.0f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        +0.5f, +0.5f, +0.0f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, +0.5f, +0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, +0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f,

        +0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        +0.5f, +0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, +0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    GLint positionSlot = glGetAttribLocation(shaderProgram_, "a_position");
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, 0);

    // color
    GLint colorSlot = glGetAttribLocation(shaderProgram_, "a_color");
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(12));
}

