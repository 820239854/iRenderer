#pragma once
#include "vector.h"
#include "triangle.h"


#define N_CUBE_VERTICES 8
extern vec3_t cube_vertices[N_CUBE_VERTICES];

#define N_CUBE_FACE8 (6*2)
extern face_t cube_faces[N_CUBE_FACE8];

typedef struct {
	vec3_t *vertices;
	face_t *faces;
	vec3_t rotation;
}mesh_t;

extern mesh_t mesh;
void load_cube_mesh_data(void);