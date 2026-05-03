#ifndef VECTOR_H
#define VECTOR_H

// what we are essentially doing here is creating 2 structs for creating vectors
// vectors are in 2d and 3d so vec2 is obvi 2d and vec3 is obvi 3d

typedef struct{
    float x;
    float y;
} Vec2_t;

typedef struct{
    float x;
    float y;
    float z;
} Vec3_t;

// 2d VECTOR functions
float vec2_len(Vec2_t v);
Vec2_t vec2_add(Vec2_t a , Vec2_t b);
Vec2_t vec2_sub(Vec2_t a, Vec2_t b);
Vec2_t vec2_mul(Vec2_t v, float factor );
Vec2_t vec2_div(Vec2_t v , float factor);
float vec2_dot(Vec2_t a , Vec2_t b);
void vec2_normalize (Vec2_t* v);




// 3d vector functions
float vec3_len(Vec3_t v);
Vec3_t vec3_add(Vec3_t a , Vec3_t b);
Vec3_t vec3_sub(Vec3_t a, Vec3_t b);
Vec3_t vec3_mul(Vec3_t v, float factor );
Vec3_t vec3_div(Vec3_t v , float factor);

Vec3_t Vec3_rotate_x(Vec3_t v, float angle);
Vec3_t Vec3_rotate_y(Vec3_t v, float angle);
Vec3_t Vec3_rotate_z(Vec3_t v, float angle);

Vec3_t vec3_cross(Vec3_t a , Vec3_t b);
float  vec3_dot(Vec3_t a , Vec3_t b);
void vec3_normalize (Vec3_t* v);


#endif
