#include "utool.h"

#include <cstdlib>
#include <SDL_log.h>
#include <SDL_image.h>


GLuint _checkGLError(const char * file, int line)
{
    GLuint err = glGetError();
    if (err > 0 )  {
        SDL_Log("GL ERROR: file:%s line:%d error:%d", file, line, err);
    }
    return err;
}


void logMatrix(const float * m, int n, int column)
{
    const int size = 2048;
    int i = 0, k = 0;
    char buff[size];

    for(i=0; i < n; ++i)
    {
        if (i % column == 0)
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


static GLuint compileShader(const char * source, int type)
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

static void linkShader(GLuint prog)
{
    GLint status;
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        SDL_Log("Can't link program");
    }
}

GLuint loadShader(const char *FS, const char *VS)
{
    // Create shader program.
    GLuint shader = glCreateProgram();

    // fragment shader
    GLuint fs = compileShader(FS, GL_FRAGMENT_SHADER);
    if (fs == 0) {
        SDL_Log("Can't compile fragment shader");
    } else {
        glAttachShader(shader, fs);
    }

    // vertex shader
    GLuint vs = compileShader(VS, GL_VERTEX_SHADER);
    if (vs == 0) {
        SDL_Log("Can't compile vertex shader");
    } else {
        glAttachShader(shader, vs);
    }

    linkShader(shader);

    glDetachShader(shader, fs);
    glDeleteShader(fs);

    glDetachShader(shader, vs);
    glDeleteShader(vs);

    return shader;
}


GLuint loadTexture(const char * imagefile)
{
#ifndef GL_BGR
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#endif
    GLuint tid = 0;
    GLenum texture_format;
    GLint ncolors;
    SDL_Surface* s = IMG_Load(imagefile);;

    // Convert SDL_Surface to OpenGL Texture
    ncolors = s->format->BytesPerPixel;
    if (ncolors == 4) {
        // alpha channel
        if (s->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;
    } else {
        if (ncolors == 3) {
            // no alpha channel
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
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels);

    SDL_FreeSurface(s);

    return tid;
}
