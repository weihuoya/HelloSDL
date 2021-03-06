#include "gltest.h"

#include "sdlrwops.h"
#include "matrix.h"

#include <cstdlib>
#include <SDL_log.h>
#include <SDL_image.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define ATTRIB_VERTEX    0
#define ATTRIB_TEXTCOORD 1
#define ATTRIB_COLOR     2

#define  CHECK_GL(...)   _checkGLError(__FILE__, __LINE__)
extern GLuint _checkGLError(const char * file, int line);
/*{
    GLuint err = glGetError();
    if (err > 0 )  {
        SDL_Log("GL ERROR: file:%s line:%d error:%d", file, line, err);
    }
    return err;
}*/


struct Vertex3D {
    float vx;
    float vy;
    float vz;
//    float tx;
//    float ty;
//    float tz;
    float rgba[4];
};


GLTest::GLTest(SDL_Window * window) : window_(window)
{
    SDL_GetCurrentDisplayMode(0, &displayMode_);

    glcontext_ = SDL_GL_CreateContext(window);

    // setup the viewport
    glViewport(0, 0, displayMode_.w, displayMode_.h);

    // clear color buffer
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram_ = 0;
    vertexBuffer_ = 0;
    indexBuffer_ = 0;
    depthBuffer_ = 0;

    rotate_ = 0;

    initialize();
}


GLTest::~GLTest()
{
    glDeleteProgram(shaderProgram_);
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteBuffers(1, &indexBuffer_);

    glDeleteRenderbuffers(1, &depthBuffer_);

    SDL_GL_DeleteContext(glcontext_);
}


void GLTest::initialize()
{
    glLoadIdentity();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // 索引缓存
    glGenBuffers(1, &indexBuffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);

    //顶点缓存
    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);

    // only draw the vertex if there isn’t someting in front of it already
    /*glGenRenderbuffers(1, &depthBuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, displayMode_.w, displayMode_.h);
    CHECK_GL();

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
    CHECK_GL();*/

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    CHECK_GL();

    // shader
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

    //SDL_Log("fShaderStr: %s", fShaderStr);
    //SDL_Log("vShaderStr: %s", vShaderStr);
    shaderProgram_ = loadShader(fShaderStr, vShaderStr);

    glUseProgram(shaderProgram_);

    free(fShaderStr);
    free(vShaderStr);

    loadData();
    CHECK_GL();
}

void GLTest::drawFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    loadMatrix();

    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_SHORT, 0);

    flush();

    rotate_ = (rotate_ + 1) % 360;
}

void GLTest::loadData()
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
        +0.5f, -0.5f, +0.0f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        +0.5f, +0.5f, +0.0f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, +0.5f, +0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, +0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f,

        +0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        +0.5f, +0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, +0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9 * 8, vertices, GL_STATIC_DRAW);

    // position
    GLint positionSlot = glGetAttribLocation(shaderProgram_, "a_position");
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, 0);

    //GLint texcoordSlot = glGetAttribLocation(shaderProgram_, "a_texcoord");
    //glEnableVertexAttribArray(texcoordSlot);
    //glVertexAttribPointer(texcoordSlot, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, 0);

    // color
    GLint colorSlot = glGetAttribLocation(shaderProgram_, "a_color");
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(12));

    // texcoord
    GLint texcoordSlot = glGetAttribLocation(shaderProgram_, "a_texcoord");
    glEnableVertexAttribArray(texcoordSlot);
    glVertexAttribPointer(texcoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, BUFFER_OFFSET(28));


    SDL_Surface * surface = NULL;
    GLuint tid = 0;

    surface = IMG_Load("floor.png");
    tid = SurfaceToTexture(surface);
    SDL_FreeSurface(surface);

    //glBindTexture(GL_TEXTURE_2D, tid);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint textureSlot = glGetUniformLocation(shaderProgram_, "u_texture");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tid);
    glUniform1i(textureSlot, 0);
}

void GLTest::loadMatrix()
{
    float ratio = (float) displayMode_.w / (float)displayMode_.h;
    float projectionMatrix[16];
    float viewMatrix[16];
    float mvpMatrix[16];
    //float rotateMatrix[16];

    float theta = rotate_ * M_PI / 180.0f;

    Matrix::frustumM(projectionMatrix, -1, 1, -1, 1, 3, 7);
    //Matrix::setRotateM(rotateMatrix, rotate_, 0, 0, -1.0f);
    Matrix::setLookAtM(viewMatrix, 0, 4.0f * sin(theta), 4.0f * cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    Matrix::multiplyMM(mvpMatrix, projectionMatrix, viewMatrix);

    // setup the viewport
    //glViewport(0, 0, displayMode_.w, displayMode_.h);
    //this->log(mvpMatrix, 16);

    GLint projectionSlot = glGetUniformLocation(shaderProgram_, "a_projection");
    glUniformMatrix4fv(projectionSlot, 1, 0, mvpMatrix);
    CHECK_GL();
}

GLuint GLTest::loadShader(const char *FS, const char *VS)
{
    // Create shader program.
    GLuint shader = glCreateProgram();

    GLuint fs = compileShader(FS, GL_FRAGMENT_SHADER);
    if (fs == 0) {
        SDL_Log("Can't compile fragment shader");
    } else {
        glAttachShader(shader, fs);
    }

    GLuint vs = compileShader(VS, GL_VERTEX_SHADER);
    if (vs == 0) {
        SDL_Log("Can't compile vertex shader");
    } else {
        glAttachShader(shader, vs);
    }

    glBindAttribLocation(shader, ATTRIB_VERTEX, "a_position");
    glBindAttribLocation(shader, ATTRIB_TEXTCOORD, "a_texcoord");
    glBindAttribLocation(shader, ATTRIB_COLOR, "a_color");

    linkShader(shader);

    glDetachShader(shader, fs);
    glDeleteShader(fs);
    glDetachShader(shader, vs);
    glDeleteShader(vs);

    //glUseProgram(shader);
    //shaderProgram_ = shader;
    return shader;
}

GLuint GLTest::compileShader(const char * source, int type)
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

void GLTest::linkShader(GLuint prog)
{
    GLint status;
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        SDL_Log("Can't link program");
    }
}


GLuint GLTest::SurfaceToTexture(SDL_Surface *surface)
{
#ifndef GL_BGR
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#endif
    GLuint tid;
    GLenum texture_format;
    GLint ncolors;
    SDL_Surface* s = surface;

    /* Convert SDL_Surface to OpenGL Texture */
    ncolors = s->format->BytesPerPixel;
    if (ncolors == 4) {
        //alpha channel
        if (s->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;
    } else {
        if (ncolors == 3) {
            //no alpha channel
            if (s->format->Rmask == 0x000000ff)
                texture_format = GL_RGB;
            else
                texture_format = GL_BGR;
        } else {
            return 0;
        }
    }

    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, s->w, s->h, 0,
            texture_format, GL_UNSIGNED_BYTE, s->pixels);

    return tid;
}


void GLTest::flush()
{
    // Swap our back buffer to the front
    SDL_GL_SwapWindow(window_);
}

void GLTest::log(const float * m, int n)
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

