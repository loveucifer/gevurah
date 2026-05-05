#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"
#include "mesh.h"

typedef struct{

    float m[4][4];

} mat4_t;

mat4_t mat4_identity(void);

mat4_t mat4_scale(float sx , float sy , float sz);

mat4_t  mat4_translaton ( float tx , float ty , float tz );

Vec4_t  mat4_mul_vec4(mat4_t m, Vec4_t v);

#endif
