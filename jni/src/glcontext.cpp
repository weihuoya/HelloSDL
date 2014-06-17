#include "glcontext.h"
#include "sdlrwops.h"
#include "matrix.h"

#include <cstdlib>
#include <SDL2/SDL_log.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define ATTRIB_VERTEX 0
#define ATTRIB_TEXTCOORD 1
#define ATTRIB_COLOR 2


#define  CHECK_GL(...)   _checkGLError(__FILE__, __LINE__)
GLuint _checkGLError(const char * file, int line)
{
    GLuint err = glGetError();
    if (err > 0 )  {
        SDL_Log("GL ERROR: file:%s line:%d error:%d", file, line, err);
    }
    return err;
}


struct Vertex2D {
    float vx;
    float vy;
    float tx;
    float ty;
    uint8_t rgba[4];
};

struct Quad2D {
    struct Vertex2D p[4];
};


struct Vertex3D {
    float vx;
    float vy;
    float vz;
//    float tx;
//    float ty;
//    float tz;
//    float rgba[4];
};

struct Quad3D {
    struct Vertex3D p[4];
};



GLContext::GLContext(SDL_Window * window) : window_(window)
{
    SDL_GetCurrentDisplayMode(0, &displayMode_);

    glcontext_ = SDL_GL_CreateContext(window);

    // setup the viewport
    glViewport(0, 0, displayMode_.w, displayMode_.h);

    //glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // Clear the color and depth buffers.
    //glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram_ = 0;
    vertexBuffer_ = 0;
    indexBuffer_ = 0;
    colorBuffer_ = 0;
    depthBuffer_ = 0;
    stencilBuffer_ = 0;

    texture_ = 0;

    initialize();
}

GLContext::~GLContext()
{
    unloadShader();

    SDL_GL_DeleteContext(glcontext_);
}

void GLContext::flush()
{
    // Swap our back buffer to the front
    SDL_GL_SwapWindow(window_);
}

void GLContext::draw()
{
    /*
    float vb[] = {
        -0.5f, +0.5f, 0.0f, 1.0f,
        +0.5f, +0.5f, 0.0f, 1.0f,
        +0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f,
        -0.8f, +0.0f, 0.0f, 1.0f,
    };

    drawQuad2D(vb);
    //drawPoint2D(vb);
    //drawLine2D(vb);
    */

    drawTest();
    //setupFrameBuffer();

    flush();
}

void GLContext::initialize()
{
    glLoadIdentity();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // 索引缓存
    glGenBuffers(1, &indexBuffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
    CHECK_GL();

    //顶点缓存
    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    CHECK_GL();

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    CHECK_GL();

    // only draw the vertex if there isn’t someting in front of it already
    /*glGenRenderbuffers(1, &depthBuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, displayMode_.w, displayMode_.h);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
    CHECK_GL();*/


    char *fShaderStr, *vShaderStr;
    SDLRWops rwops;
    size_t filesize, readsize;

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

    //shader
    loadShader(fShaderStr, vShaderStr);

    free(fShaderStr);
    free(vShaderStr);
}


void GLContext::drawLine2D(const float * vb)
{
    int i;
    uint32_t color = 0xFFFF00;
    const int max_quad = 32;

    GLushort indices[4];
    for (i = 0; i < 4; ++i) {
        indices[i] = i;
    }
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    struct Quad2D qoad;
    for (i = 0; i < 4; ++i) {
        // vertex
        qoad.p[i].vx = vb[i*4+0];
        qoad.p[i].vy = vb[i*4+1];
        // textcoord
        qoad.p[i].tx = vb[i*4+2];
        qoad.p[i].ty = vb[i*4+3];
        // color
        qoad.p[i].rgba[0] = (color >> 16) & 0xff;
        qoad.p[i].rgba[1] = (color >> 8) & 0xff;
        qoad.p[i].rgba[2] = (color) & 0xff;
        qoad.p[i].rgba[3] = (color >> 24) & 0xff;
    }
    //glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(qoad), &qoad, GL_DYNAMIC_DRAW);


    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex2D), BUFFER_OFFSET(0));

    glEnableVertexAttribArray(ATTRIB_TEXTCOORD);
    glVertexAttribPointer(ATTRIB_TEXTCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex2D), BUFFER_OFFSET(8));

    glEnableVertexAttribArray(ATTRIB_COLOR);
    glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex2D), BUFFER_OFFSET(16));

    glDrawElements(GL_LINES, 4, GL_UNSIGNED_SHORT, 0);
}

void GLContext::drawQuad2D(const float * vb)
{
    int i;
    uint32_t color[4] = {0xFFFF00, 0xFF00FF, 0xFF0000, 0x00FF00};
    const int max_quad = 32;

    GLushort indices[6 * max_quad];
    for (i = 0; i < max_quad; ++i) {
        indices[i*6+0] = i*4+0;
        indices[i*6+1] = i*4+1;
        indices[i*6+2] = i*4+2;
        indices[i*6+3] = i*4+0;
        indices[i*6+4] = i*4+2;
        indices[i*6+5] = i*4+3;
    }
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    struct Quad2D qoad;
    for (i = 0; i < 4; ++i) {
        // vertex
        qoad.p[i].vx = vb[i*4+0];
        qoad.p[i].vy = vb[i*4+1];
        // textcoord
        qoad.p[i].tx = vb[i*4+2];
        qoad.p[i].ty = vb[i*4+3];
        // color
        qoad.p[i].rgba[0] = (color[i] >> 16) & 0xff;
        qoad.p[i].rgba[1] = (color[i] >> 8) & 0xff;
        qoad.p[i].rgba[2] = (color[i]) & 0xff;
        qoad.p[i].rgba[3] = (color[i] >> 24) & 0xff;
    }
    //glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(qoad), &qoad, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex2D), BUFFER_OFFSET(0));

    glEnableVertexAttribArray(ATTRIB_TEXTCOORD);
    glVertexAttribPointer(ATTRIB_TEXTCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex2D), BUFFER_OFFSET(8));

    glEnableVertexAttribArray(ATTRIB_COLOR);
    glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex2D), BUFFER_OFFSET(16));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}


void GLContext::drawTest()
{
    GLushort indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 6, 5, 4, 7, 6,
        2, 7, 3, 7, 6, 2,
        0, 4, 1, 4, 1, 5,
        6, 2, 1, 1, 6, 5,
        0, 3, 7, 0, 7, 4,
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * 6, indices, GL_STATIC_DRAW);

    GLfloat vertices[] = {
        +0.5f, -0.5f, +0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        +0.5f, +0.5f, +0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, +0.5f, +0.0f,  0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, +0.0f,  1.0f, 1.0f, 0.0f, 1.0f,

        +0.5f, -0.5f, -1.0f,  0.0f, 1.0f, 1.0f, 1.0f,
        +0.5f, +0.5f, -1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, +0.5f, -1.0f,  1.0f, 0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -1.0f,  1.0f, 0.5f, 0.0f, 1.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 7 * 8, vertices, GL_STATIC_DRAW);

    GLint positionSlot = glGetAttribLocation(shaderProgram_, "a_position");
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, 0);

    GLint texcoordSlot = glGetAttribLocation(shaderProgram_, "a_texcoord");
    glEnableVertexAttribArray(texcoordSlot);
    glVertexAttribPointer(texcoordSlot, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, 0);

    GLint colorSlot = glGetAttribLocation(shaderProgram_, "a_color");
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, BUFFER_OFFSET(12));

    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_SHORT, 0);
    CHECK_GL();
}


void GLContext::log(const float * m, int n)
{
    const int size = 2048;
    int i = 0, k = 0;
    char buff[size];

    for(i=0; i < n; ++i)
    {
        if (i % 4 == 0)
        {
            buff[k++] = '\n';
        }

        k += snprintf(buff + k, size-k, "%.02f", m[i]);
        buff[k++] = ',';
        buff[k++] = ' ';
    }

    buff[k] = 0;
    SDL_Log("matrix:%s", buff);
}

void GLContext::loadMatrix()
{
    float ratio = (float) displayMode_.w / (float)displayMode_.h;
    float projectionMatrix[16];
    float viewMatrix[16];
    float mvpMatrix[16];
    float rotateMatrix[16];

    Matrix::frustumM(projectionMatrix, -ratio, ratio, -1, 1, 3, 7);
    Matrix::setRotateM(rotateMatrix, 30.0f, 0, 0, -1.0f);
    Matrix::setLookAtM(viewMatrix, 0, 0, -3, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    Matrix::multiplyMM(mvpMatrix, rotateMatrix, viewMatrix);

    // setup the viewport
    //glViewport(0, 0, displayMode_.w, displayMode_.h);
    this->log(mvpMatrix, 16);

    GLint projectionSlot = glGetUniformLocation(shaderProgram_, "a_projection");
    CHECK_GL();

    glUniformMatrix4fv(projectionSlot, 1, 0, mvpMatrix);
    CHECK_GL();
}

void GLContext::setupFrameBuffer()
{
    GLint savedFBO = 0;
    GLint width = displayMode_.w, height = displayMode_.h;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &savedFBO);

    glGenFramebuffers(1, &frameBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    glClear(0);
    //glViewport(0, 0, width, height);

    /*
    // Generates the name/id, creates and configures the Color Render Buffer.
    glGenRenderbuffers(1, &colorBuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer_);

    // Generates the name/id, creates and configures the Depth Render Buffer.
    glGenRenderbuffers(1, &depthBuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);

    // Generates the name/id, creates and configures the Stencil Render Buffer.
    glGenRenderbuffers(1, &stencilBuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer_);
    */

    glEnable(GL_TEXTURE_2D);
    // The texture we're going to render to
    glGenTextures(1, &texture_);
    // generate texture
    glBindTexture(GL_TEXTURE_2D, texture_);
    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // parameters - we have to make sure we clamp the textures to the edges
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);

    // Set the list of draw buffers.
    //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // "1" is the size of DrawBuffers
    //glDrawBuffers(1, DrawBuffers);


    SDL_Log("call drawTest()");

    drawTest();

    // Always check that our framebuffer is ok
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status == GL_FRAMEBUFFER_COMPLETE) {
        SDL_Log("Single FBO setup successfully");
    } else {
        SDL_Log("Problem in setup FBO texture: %d", status);
        return;
    }

    SDL_Log("call glBindFramebuffer(), saved= %d", savedFBO);
    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, savedFBO);
    glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    SDL_Log("call glClear()");

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    GLuint textureSlot = glGetUniformLocation(shaderProgram_, "u_texture");

    SDL_Log("u_texture id: %d", textureSlot);
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(textureSlot, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    SDL_Log("finished setupFrameBuffer()");
}

void GLContext::loadShader(const char *FS, const char *VS)
{
    // Create shader program.
    shaderProgram_ = glCreateProgram();

    GLuint fs = compileShader(FS, GL_FRAGMENT_SHADER);
    if (fs == 0) {
        SDL_Log("Can't compile fragment shader");
    } else {
        glAttachShader(shaderProgram_, fs);
    }

    GLuint vs = compileShader(VS, GL_VERTEX_SHADER);
    if (vs == 0) {
        SDL_Log("Can't compile vertex shader");
    } else {
        glAttachShader(shaderProgram_, vs);
    }

    glBindAttribLocation(shaderProgram_, ATTRIB_VERTEX, "a_position");
    glBindAttribLocation(shaderProgram_, ATTRIB_TEXTCOORD, "a_texcoord");
    glBindAttribLocation(shaderProgram_, ATTRIB_COLOR, "a_color");

    linkShader(shaderProgram_);

    //additive = glGetUniformLocation(shaderProgram_, "additive");

    glDetachShader(shaderProgram_, fs);
    glDeleteShader(fs);
    glDetachShader(shaderProgram_, vs);
    glDeleteShader(vs);

    glUseProgram(shaderProgram_);

    loadMatrix();
}

void GLContext::unloadShader()
{
    glDeleteProgram(shaderProgram_);

    glDeleteBuffers(1, &vertexBuffer_);

    glDeleteBuffers(1, &indexBuffer_);

    //glDeleteRenderbuffers(1, &colorBuffer_);
    glDeleteRenderbuffers(1, &depthBuffer_);
    //glDeleteRenderbuffers(1, &stencilBuffer_);

    glDeleteFramebuffers(1, &frameBuffer_);

    glDeleteTextures(1, &texture_);
}

GLuint GLContext::compileShader(const char * source, int type)
{
    GLint status;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        char buf[1024];
        GLint len;
        glGetShaderInfoLog(shader, 1024, &len, buf);

        SDL_Log("compile failed:%s\nsource:\n %s\n", buf, source);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void GLContext::linkShader(GLuint prog)
{
    GLint status;
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        SDL_Log("Can't link program");
    }
}


