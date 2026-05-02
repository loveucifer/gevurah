#ifndef MESH_H
#define MESH_H
#include "triangle.h"
#include "vector.h"


#define N_CUBE_VERTICES 8
extern Vec3_t cube_vertices[N_CUBE_VERTICES];


#define N_CUBE_FACES (6*2) // 6 cube faces 2 traingles per face
extern face_t cube_faces[N_CUBE_FACES];

//////////////////////////////////////////////////////////////////////
/// defines a struct for dynamic size meshes with array of vertices and faces
/// ????????????????????????????????????????????????????????

typedef struct {
    Vec3_t* vertices;  // dynamic array of vertices and faces
    face_t* faces;
    Vec3_t rotation;  /// rotation with x y z value
} mesh_t;

extern mesh_t mesh;  // global mesh variable


void load_cube (void);
void load_obj_file(char* filename);


#endif


//
// no of arrays in C is fixed unlike doing vector with c++
// so we go onto make yet another dynamic allocator hmm
//
// this just means we cant do
// hello = ["hi","hey"]
// hello.push_back ("hiee")
