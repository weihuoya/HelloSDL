#include "glcube.h"


#define TEX_COORD_MAX   4.0f


typedef struct {
    float Position[3];
    float Color[4];
    float TexCoord[2];
} Vertex;



GLCube::GLCube() : vboids_(new GLuint[2]), count_(0)
{
}

GLCube::~GLCube()
{
    glDeleteBuffers(2, vboids_);
    delete[] vboids_;
}

void GLCube::init()
{
    // Generate 2 VBOs
    glGenBuffers(2, vboids_);
}

void GLCube::load1()
{
    const GLushort indices[] = {
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
        20, 21, 22, 22, 23, 20,
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboids_[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    const Vertex vertices[] = {
        // Front
        {{+1, -1, +0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, +1, +0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, +1, +0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, +0}, {1, 0, 0, 1}, {0, 0}},
        // Back
        {{+1, -1, -2}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, +1, -2}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, +1, -2}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, -2}, {1, 0, 0, 1}, {0, 0}},
        // Left
        {{-1, -1, +0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{-1, +1, +0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, +1, -2}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, -2}, {1, 0, 0, 1}, {0, 0}},
        // Right
        {{+1, -1, -2}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, +1, -2}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{+1, +1, +0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{+1, -1, +0}, {1, 0, 0, 1}, {0, 0}},
        // Top
        {{+1, +1, +0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, +1, -2}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, +1, -2}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, +1, +0}, {1, 0, 0, 1}, {0, 0}},
        // Bottom
        {{+1, -1, -2}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
        {{+1, -1, +0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
        {{-1, -1, +0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
        {{-1, -1, -2}, {1, 0, 0, 1}, {0, 0}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, vboids_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    count_ = sizeof(indices)/sizeof(indices[0]);
    SDL_Log("elements: %d, indices: %d, vertices: %d", count_, sizeof(indices), sizeof(vertices));
}

void GLCube::load2()
{
    const GLushort indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 6, 5, 4, 7, 6,
        2, 7, 3, 7, 6, 2,
        0, 4, 1, 4, 1, 5,
        6, 2, 1, 1, 6, 5,
        0, 3, 7, 0, 7, 4,
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboids_[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    const Vertex vertices[] = {
        {{+0.5f, -0.5f, +0.0f},  {1.0f, 0.0f, 0.0f, 1.0f},  {1.0f, 0.0f}},
        {{+0.5f, +0.5f, +0.0f},  {1.0f, 0.0f, 0.0f, 1.0f},  {1.0f, 1.0f}},
        {{-0.5f, +0.5f, +0.0f},  {0.0f, 1.0f, 0.0f, 1.0f},  {0.0f, 1.0f}},
        {{-0.5f, -0.5f, +0.0f},  {0.0f, 1.0f, 0.0f, 1.0f},  {0.0f, 0.0f}},

        {{+0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f, 1.0f},  {1.0f, 0.0f}},
        {{+0.5f, +0.5f, -0.5f},  {1.0f, 0.0f, 0.0f, 1.0f},  {1.0f, 1.0f}},
        {{-0.5f, +0.5f, -0.5f},  {0.0f, 1.0f, 0.0f, 1.0f},  {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f, 1.0f},  {0.0f, 0.0f}},
    };
    glBindBuffer(GL_ARRAY_BUFFER, vboids_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    count_ = sizeof(indices)/sizeof(indices[0]);
    SDL_Log("elements: %d, indices: %d, vertices: %d", count_, sizeof(indices), sizeof(vertices));
}


void GLCube::draw(GLuint shaderProgram)
{
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboids_[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vboids_[1]);

    // position
    GLint positionSlot = glGetAttribLocation(shaderProgram, "a_position");
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    // color
    GLint colorSlot = glGetAttribLocation(shaderProgram, "a_color");
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(12));

    // texcoord
    GLint texcoordSlot = glGetAttribLocation(shaderProgram, "a_texcoord");
    glEnableVertexAttribArray(texcoordSlot);
    glVertexAttribPointer(texcoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(28));

    // Draw cube geometry using indices from VBO 1
    //glDrawElements(GL_TRIANGLES, count_, GL_UNSIGNED_BYTE, 0);
    glDrawElements(GL_TRIANGLES, count_, GL_UNSIGNED_SHORT, 0);
}

