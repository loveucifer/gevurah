#include "vector.h"
#include <math.h>


// this is very bad practice ngl

float Vec3_len(Vec3_t v){
    return sqrt(v.x * v.x + v.y * v.y );
};

Vec3_t Vec3_add(Vec3_t a, Vec3_t b){
    Vec3_t result = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return result;
}

Vec3_t Vec3_sub(Vec3_t a, Vec3_t b){
    Vec3_t result = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return result;
}

Vec3_t Vec3_mul(Vec3_t v, float factor);{
    Vec3_t result = {
        .x = v.x * factor,
        .y = v.y * factor
    };
    return result;
}


Vec3_t Vec3_div(Vec3_t v, float factor){
    Vec3_t result = {
        .x = v.x / factor,
        .y = v.y / factor
    };
    return result;
}

//////////////////////////
// 3d vector functions
//////////////////////////

float vec3_len(Vec3_t v){
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
};

Vec3_t vec3_add(Vec3_t a, Vec3_t b) {
  Vec3_t result = {
      .x = a.x + b.x,
      .y = a.y + b.y,
      .z = a.z + b.z
  };
  return result;
}

Vec3_t vec3_sub(Vec3_t a, Vec3_t b) {
  Vec3_t result = {
      .x = a.x - b.x,
      .y = a.y - b.y,
      .z = a.z - b.z
  };
  return result;
}

Vec3_t vec3_mul(Vec3_t v, float factor){
  Vec3_t result = {
      .x = v.x * factor,
      .y = v.y * factor,
      .z = v.z *factor
  };
  return result;
}

Vec3_t vec3_div(Vec3_t v, float factor) {
  Vec3_t result = {
      .x = v.x / factor,
      .y = v.y / factor,
      .z = v.z / factor
  };
  return result;
}

Vec3_t Vec3_rotate_x(Vec3_t v, float angle) {
  Vec3_t rotated_vector = {.x = v.x,
                           .y = v.y * cos(angle) - v.z * sin(angle),
                           .z = v.y * sin(angle) + v.z * cos(angle)};
  return rotated_vector;
}

Vec3_t Vec3_rotate_y(Vec3_t v, float angle) {
  Vec3_t rotated_vector = {.x = v.x * cos(angle) - v.z * sin(angle),
                           .y = v.y,
                           .z = v.x * sin(angle) + v.z * cos(angle)};
  return rotated_vector;
}

Vec3_t Vec3_rotate_z(Vec3_t v, float angle) {
  Vec3_t rotated_vector = {.x = v.x * cos(angle) - v.y * sin(angle),
                           .y = v.x * sin(angle) + v.y * cos(angle),
                           .z = v.z};
  return rotated_vector;
}
