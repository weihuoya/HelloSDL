#include "glcontext.h"
#include "sdlrwops.h"
#include "matrix.h"

#include <SDL_log.h>


// model
static float modelMatrix[16];
// view
static float viewMatrix[16];
// projection
static float projectionMatrix[16];
// projection * view * model
static float mvpMatrix[16];



GLContext::GLContext() :
    window_(0), cube_(0),
    shaderProgram_(0), textureBuffer_(0), transform_(0)
{
}

GLContext::~GLContext()
{
    if(shaderProgram_) glDeleteProgram(shaderProgram_);
    if(textureBuffer_) glDeleteTextures(1, &textureBuffer_);

    if(cube_) delete cube_;
    if(transform_) delete transform_;
    if(glcontext_) SDL_GL_DeleteContext(glcontext_);
}

GLContext * GLContext::instance()
{
    static GLContext context;
    return &context;
}


void GLContext::initialize(SDL_Window * window)
{
    SDL_GetCurrentDisplayMode(0, &displayMode_);
    glcontext_ = SDL_GL_CreateContext(window);
    cube_ = new GLCube();
    transform_ = new GLTransform();
    window_ = window;


    glEnable(GL_BLEND);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // setup the viewport
    glViewport(0, 0, displayMode_.w, displayMode_.h);

    // projection
    float aspect = (float)displayMode_.w / (float)displayMode_.h;
    Matrix::setIdentityM(projectionMatrix);
    //Matrix::perspectiveM(projectionMatrix, 45.0f, aspect, 3.0f, 7.0f);
    Matrix::frustumM(projectionMatrix, -1, 1, -1, 1, 2, 10);

    // view
    //Matrix::setIdentityM(viewMatrix);
    Matrix::setLookAtM(viewMatrix, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // load
    this->loadShader();
    this->loadTexture();
    CHECK_GL();

    cube_->init();
    cube_->load1();
    CHECK_GL();

    //this->loadMatrix();
    //logMatrix(mvpMatrix, sizeof(mvpMatrix)/sizeof(mvpMatrix[0]));
}


void GLContext::loadShader()
{
    SDLRWops rwops;
    size_t filesize, readsize;
    char *fShaderStr, *vShaderStr;

    // fragment shader
    rwops.fromFile("fragment.glsl", "r");
    filesize = rwops.size();
    fShaderStr = (char*)malloc(filesize+1);
    fShaderStr[filesize] = 0;
    readsize = rwops.read(fShaderStr, filesize);
    rwops.close();

    // vertex shader
    rwops.fromFile("vertex.glsl", "r");
    filesize = rwops.size();
    vShaderStr = (char*)malloc(filesize+1);
    vShaderStr[filesize] = 0;
    readsize = rwops.read(vShaderStr, filesize);
    rwops.close();

    shaderProgram_ = ::loadShader(fShaderStr, vShaderStr);
    glUseProgram(shaderProgram_);

    free(fShaderStr);
    free(vShaderStr);
}


void GLContext::loadTexture()
{
    //glEnable(GL_TEXTURE_2D);

    GLuint tid = ::loadTexture("floor.png");
    GLint textureSlot = glGetUniformLocation(shaderProgram_, "u_texture");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tid);
    glUniform1i(textureSlot, 0);

    textureBuffer_ = tid;

    // Set nearest filtering mode for texture minification
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set bilinear filtering mode for texture magnification
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void GLContext::loadMatrix()
{
    // model
    //Matrix::setIdentityM(modelMatrix);
    //Matrix::translateM(modelMatrix, 0.0f, 0.0f, -4.0f);

    transform_->getModelViewMatrix(modelMatrix);

    // projection * view * model
    //Matrix::multiplyMM(temporaryMatrix_, viewMatrix, modelMatrix);
    Matrix::multiplyMM(mvpMatrix, projectionMatrix, modelMatrix);

    //Matrix::setIdentityM(mvpMatrix);

    // mvp
    GLint mvpSlot = glGetUniformLocation(shaderProgram_, "u_mvpmatrix");
    glUniformMatrix4fv(mvpSlot, 1, 0, mvpMatrix);
}


void GLContext::drawFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    loadMatrix();

    cube_->draw(shaderProgram_);

    flush();
}


void GLContext::flush()
{
    // Swap our back buffer to the front
    SDL_GL_SwapWindow(window_);
}


void GLContext::incRotate(float rotateX, float rotateY, float rotateZ)
{
    SDL_Log("rotate: (%f, %f)", rotateX, rotateY, rotateZ);
}

void GLContext::incScale(float scale)
{
    SDL_Log("scale: %f", scale);
}

void GLContext::incTranslate(float x, float y)
{
    SDL_Log("translate: (%f, %f)", x, y);
}
