#ifndef CLIPPPING_H
#define CLIPPPING_H
#include "vector.h"


#define MAX_NO_OF_POLYGON_VERTICES 10

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE,
};

typedef struct{
    Vec3_t point;
    Vec3_t normal;
} plane_t;

typedef struct{

    Vec3_t vertices[MAX_NO_OF_POLYGON_VERTICES];
    int no_of_vertices;


} polygon_t ;

void init_frustum_planes(float fov , float z_near , float z_far);

void clip_polygon(polygon_t* polygon);

polygon_t create_poly_from_triangle (Vec3_t v0, Vec3_t v1 , Vec3_t v2 );
#endif
