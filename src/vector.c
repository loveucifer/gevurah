#include "vector.h"
#include <math.h>


// this is very bad practice ngl


////////////////////////////////////
////// vec2 ///////////////////////
//////////////////////////////////

float vec2_len(Vec2_t v){
    return sqrt(v.x * v.x + v.y * v.y );
};

Vec2_t vec2_add(Vec2_t a, Vec2_t b){
    Vec2_t result = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return result;
}

Vec2_t vec2_sub(Vec2_t a, Vec2_t b){
    Vec2_t result = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return result;
}

Vec2_t vec2_mul(Vec2_t v, float factor){
    Vec2_t result = {
        .x = v.x * factor,
        .y = v.y * factor
    };
    return result;
}


Vec2_t vec2_div(Vec2_t v, float factor){
    Vec2_t result = {
        .x = v.x / factor,
        .y = v.y / factor
    };
    return result;
}

float vec2_dot(Vec2_t a, Vec2_t b){
    return ( a.x * b.x) + (a.y * b.y);
}

void vec2_normalize(Vec2_t *v){
    float length = sqrt(v->x * v->x + v->y * v->y );
    v->x /= length;
    v->y /= length;
}

Vec2_t vec2_new(float x,  float y){
    Vec2_t result = {x,y};
    return result;
}

Vec2_t vec2_clone(Vec2_t *v){
   Vec2_t result = {v->x , v->y};
   return  result;
}


///////////////////////////////////////
//// vec3 ////////////////////////////
/////////////////////////////////////

Vec3_t vec3_new(float x,  float y,float z){
    Vec3_t result = {x,y,z};
    return result;
}

Vec3_t vec3_clone(Vec3_t *v){
   Vec3_t result = {v->x , v->y, v->z};
   return  result;
}

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

Vec3_t vec3_cross(Vec3_t a, Vec3_t b){
    Vec3_t result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return result;
}

float vec3_dot(Vec3_t a, Vec3_t b){
    return ( a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}


void vec3_normalize(Vec3_t *v){
    float length = sqrt(v->x * v->x + v->y * v->y +  v->z * v->z );
    if (length == 0.0f) return;
    v->x /= length;
    v->y /= length;
    v->z /= length;
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


/*///////////////////////////////////////////////
////// VECTOR CONVERSIONS////////////////////////
//////////////////////////////////////////////*/


Vec4_t vec4_from_vec3(Vec3_t v){
    Vec4_t result = {v.x , v.y , v.z, 1.0};
    return result;
}

Vec3_t vec3_from_vec4(Vec4_t v){
    Vec3_t result = {v.x , v.y , v.z};
    return result;
}

Vec2_t vec2_from_vec4(Vec4_t v){
    Vec2_t result = {v.x , v.y};
    return result;
}
