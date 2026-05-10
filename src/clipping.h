#ifndef CLIPPPING_H
#define CLIPPPING_H
#include "texture.h"
#include "triangle.h"
#include "vector.h"


#define MAX_NO_OF_POLYGON_VERTICES 10
#define MAX_NO_OF_POLYGON_TRIANGLES 10  // NO OF POLYGON TRIANGLES faces will be always no of vertices - 2

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
    tex2_t texcoordiantes[MAX_NO_OF_POLYGON_VERTICES];
    int no_of_vertices;

} polygon_t ;

void init_frustum_planes(float fovy, float fovx , float z_near , float z_far);

void clip_polygon(polygon_t* polygon);

polygon_t create_poly_from_triangle (Vec3_t v0, Vec3_t v1 , Vec3_t v2 , tex2_t t0 , tex2_t t1 , tex2_t t2);

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int *no_of_triangles);

#endif
