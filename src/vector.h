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

Vec3_t Vec3_rotate_x(Vec3_t v, float angle);
Vec3_t Vec3_rotate_y(Vec3_t v, float angle);
Vec3_t Vec3_rotate_z(Vec3_t v, float angle);
// todo // add functions to manipualte these vectors
//

#endif
