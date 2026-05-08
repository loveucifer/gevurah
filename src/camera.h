#ifndef  CAMERA_H
#define  CAMERA_H
#include "vector.h"

typedef  struct{
    Vec3_t position;
    Vec3_t direction;
    Vec3_t forward_velocity;  // vector of how much forward or backward
    float yaw;  // how much we are rotating around y axis or yaw rotation
} camera_t ;

extern camera_t camera;

#endif
