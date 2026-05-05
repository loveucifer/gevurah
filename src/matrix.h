#ifndef MATRIX_H
#define MATRIX_H

typedef struct{

    float m[4][4];

} mat4_t;

mat4_t mat4_identity(void);

mat4_t mat4_scale(float sx , float sy , float sz);


#endif
