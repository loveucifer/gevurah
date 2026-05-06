#ifndef  LIGHT_H
#define LIGHT_H

#include "vector.h"
#include <stdint.h>


typedef struct{
    Vec3_t direction;
} light_t;

extern light_t light;
uint32_t light_intensity_apply (uint32_t original_color , float percentage_factor);

#endif
