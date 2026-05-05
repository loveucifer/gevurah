#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"

typedef struct{

    float m[4][4];

} mat4_t;

mat4_t mat4_identity(void);

mat4_t mat4_scale(float sx , float sy , float sz);

mat4_t  mat4_translaton ( float tx , float ty , float tz );

mat4_t  mat4_rotation_x (float angle);
mat4_t  mat4_rotation_y (float angle );
mat4_t  mat4_rotation_z (float angle );

Vec4_t  mat4_mul_vec4(mat4_t m, Vec4_t v);

mat4_t  mat4_t_mul_mat4(mat4_t a, mat4_t b);

mat4_t mat4_perspective(float fov, float aspect , float znear , float zfar );

Vec4_t mat4_t_mul_vec4_t(mat4_t mat_proj , Vec4_t );

#endif
