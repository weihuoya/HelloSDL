#ifndef EJOY_2D_SHADER_H
#define EJOY_2D_SHADER_H

#include <stdint.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define PROGRAM_DEFAULT -1
#define PROGRAM_PICTURE 0
#define PROGRAM_TEXT 1
#define PROGRAM_TEXT_EDGE 2

void shader_init();
void shader_load(int prog, const char *fs, const char *vs);
void shader_unload();
void shader_blend(int m1, int m2);
void shader_defaultblend();
void shader_texture(int id);

void shader_drawpoint(const float *vb, uint32_t color, uint32_t size);
void shader_drawline(const float *vb, uint32_t color, uint32_t size, uint32_t type);

void shader_draw(const float vb[16], uint32_t color);
void shader_drawpolygon(int n, const float *vb, uint32_t color);

void shader_program(int n, uint32_t arg);
void shader_flush();

// 还原当前的环境，比如rt渲染之后
void shader_reset();

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif
