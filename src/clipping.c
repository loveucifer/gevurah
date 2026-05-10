#include "clipping.h"
#include "vector.h"
#include <math.h>

#define NUM_PLANES 6

plane_t frustum_planes[NUM_PLANES];

void init_frustum_planes(float fov , float z_near , float z_far){
    ///////////////////////////////////////////////////////////////////////////////
    // Frustum planes are defined by a point and a normal vector
    ///////////////////////////////////////////////////////////////////////////////
    // Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
    // Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
    // Top plane    :  P=(0, 0, 0),     N=(0, -cos(fov/2), sin(fov/2))
    // Bottom plane :  P=(0, 0, 0),     N=(0, cos(fov/2), sin(fov/2))
    // Left plane   :  P=(0, 0, 0),     N=(cos(fov/2), 0, sin(fov/2))
    // Right plane  :  P=(0, 0, 0),     N=(-cos(fov/2), 0, sin(fov/2))
    ///////////////////////////////////////////////////////////////////////////////
    //
    //           /|\
    //         /  | |
    //       /\   | |
    //     /      | |
    //  P*|-->  <-|*|   ----> +z-axis
    //     \      | |
    //       \/   | |
    //         \  | |
    //           \|/
    //
    ///////////////////////////////////////////////////////////////////////////////
	float cos_half_fov = cos(fov / 2);
	float sin_half_fov = sin(fov / 2);

	frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

	frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;

    }

polygon_t create_poly_from_triangle(Vec3_t v0, Vec3_t v1, Vec3_t v2){
        polygon_t polygon = {
            .vertices = {v0, v1 , v2},
            .no_of_vertices = 3
        };
        return polygon;
    }

void clip_polygon_against_plane(polygon_t* polygon, int plane){

        Vec3_t plane_point = frustum_planes[plane].point;
        Vec3_t plane_normal = frustum_planes[plane].point;

        // declare a static array of inside vertices
        Vec3_t inside_vertices[MAX_NO_OF_POLYGON_VERTICES];
        int no_inside_vertices = 0;

        Vec3_t* current_vertex  = &polygon->vertices[0];
        Vec3_t* previous_vertex = &polygon->vertices[polygon->no_of_vertices-1] ;

        float current_dot = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);
        float previous_dot = vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_normal);

        while (current_vertex != &polygon->vertices[polygon->no_of_vertices]) {

            current_dot = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);

            if (current_dot * previous_dot < 0) {

            // find interpolation factor
            float t = previous_dot / (previous_dot - current_dot);
                                                                                 // calculate intersection point I = Q1 + T (Q2-Q1)
            // insert intersection point into list of inside vertices
            Vec3_t intersection_point = vec3_clone(current_vertex);              // I = Qc
            intersection_point = vec3_sub(intersection_point, *previous_vertex); // I = (Qc - Qp)
            intersection_point = vec3_mul(intersection_point, t);                // I = T (Qc - Qp)
            intersection_point = vec3_add(intersection_point, *previous_vertex); // I = Qp + T (Qc-Qp)

            inside_vertices[no_inside_vertices] = vec3_clone(&intersection_point);
            no_inside_vertices++;

            }

            // current vertex inside plane
            if (current_dot >0 ) {
                    // add current dot inside the list of inside vertices
                    inside_vertices[no_inside_vertices] = vec3_clone(current_vertex);
                    no_inside_vertices++;
            }

            // move to next vertex
            previous_dot = current_dot;
            previous_vertex = current_vertex;
            current_vertex++;
        }

        // copy list of inside vertices into destination polygon which is out parameter
        for (int i = 0;  i < no_inside_vertices ; i++) {
            polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
        }
        polygon->no_of_vertices = no_inside_vertices;
    }

void clip_polygon(polygon_t* polygon){
        clip_polygon_against_plane(polygon , LEFT_FRUSTUM_PLANE);
        clip_polygon_against_plane(polygon , RIGHT_FRUSTUM_PLANE);
        clip_polygon_against_plane(polygon , TOP_FRUSTUM_PLANE);
        clip_polygon_against_plane(polygon , BOTTOM_FRUSTUM_PLANE);
        clip_polygon_against_plane(polygon , NEAR_FRUSTUM_PLANE);
        clip_polygon_against_plane(polygon , FAR_FRUSTUM_PLANE);

    }
