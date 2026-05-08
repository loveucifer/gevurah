#ifndef  CAMERA_H
#define  CAMERA_H
#include "vector.h"

typedef  struct{
    Vec3_t position;
    Vec3_t direction;
} camera_t ;

extern camera_t camera;

#endif
